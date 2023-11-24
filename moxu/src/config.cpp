#include"config.h"

namespace moxu{
    Config::ConfigVarMap Config::s_datas; //类中只是声明 所有的非字面值的静态变量都只能在类外进行定义和初始化

    ConfigVarBase::ptr Config::LookupBase(const std::string& name){
    std::cout<<"the "<<__func__<<" is running......"<<"The line is "<<__LINE__<<std::endl;

        std::cout<<"\n"<<std::endl<<"Next is the contexts in the map s_datas :"<<std::endl;
        for(auto& it : s_datas){
            std::cout<<it.first<<std::endl;
        }
        std::cout<<"The contexts in the map s_datas is over"<<std::endl;
        auto it  =  s_datas.find(name);
        return it == s_datas.end()? nullptr:it->second;
    }

/* "A.B",10
    A:
        B:10
        c:str
*/

    static void ListAllMember(const std::string& prefix,
                              const YAML::Node& node,
                              std::list<std::pair<std::string,const YAML::Node> >& output){
                    if(prefix.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ._0123456789")
                        !=std::string::npos){ //有除了这些之外的非法字符
                            MOXU_LOG_ERROR(MOXU_LOG_ROOT())<<"Config invalid name : "<< prefix <<" : " <<node;
                            return;
                        }
             
                    output.emplace_back(std::make_pair(prefix,node));
                    if(node.IsMap()){
                        for( YAML::const_iterator it = node.begin();it!=node.end();++it){
                                ListAllMember(prefix.empty() ? it->first.Scalar() : prefix + "." +  
                                it->first.Scalar(),it->second,
                                output);
                  
                        }
                    }
    }

    void Config::LoadFromYaml(const YAML::Node& root){
        std::list<std::pair<std::string,const YAML::Node> > all_nodes;
       ListAllMember("",root,all_nodes);

        for(std::pair<std::string,const YAML::Node>& i : all_nodes){
            std::string key = i.first;
            if(key.empty()){
                continue;
            }
            std::cout<<"the "<<__func__<<" is running......"<<std::endl;
            std::transform(key.begin(),key.end(),key.begin(),tolower);
            ConfigVarBase::ptr var = LookupBase(key);
            std::cout<<"the "<<__func__<<" is running......"<<"The line is "<<__LINE__
            <<" The key is "<<key<<std::endl;
            if(var){
               std::cout<<"the "<<__func__<<" is running......"<<"The line is "<<__LINE__
               <<"The var is "<< var.get()->get_Name()<<std::endl;
                if(i.second.IsScalar()){ //如果是标量
                    var->fromString(i.second.Scalar());
                }else{
                    std::stringstream ss;
                    ss<<i.second;
                    var->fromString(ss.str());
                }
            }
        }

    }

}
