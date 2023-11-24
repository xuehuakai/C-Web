#ifndef __MOXU_CONFIG_H__
#define __MOXU_CONFIG_H__

#include<memory>
#include<sstream>
#include<string>
#include<boost/lexical_cast.hpp>
#include<yaml-cpp/yaml.h>
#include<typeinfo>
#include"log.h"
#include<vector>
#include<map>
#include<set>
#include<list>
#include<unordered_map>
#include<unordered_set>
#include<functional>

/*lookup层转换为小写 -- 记*/

namespace moxu{

//配置项基类
class ConfigVarBase{  //抽象配置项
public:
    typedef std::shared_ptr<ConfigVarBase> ptr;
    ConfigVarBase(const std::string & name,const std::string & description = ""):
            m_name(name),
            m_description(description){ 
                std::transform(m_name.begin(),m_name.end(),m_name.begin(),
                [](char& c){
                    return std::tolower(c);
                }); //直接转换为小写
            }

    virtual ~ConfigVarBase() {}   //方便释放内存

    const std::string get_Name() const { return m_name; }
    const std::string get_Description() const { return m_description; }

    virtual std::string toString() = 0;
    virtual bool fromString(const std::string& val) = 0;
    virtual std::string getTypeName() const = 0;
protected:
    std::string m_name;
    std::string m_description;
};

//基础类型的转换类 使用boost::lexical_cast<dis>(src)
//F from_type T to_type
template<class F,class T>
class LexicalCast{
public:
    T operator()(const F& v){
            return boost::lexical_cast<T>(v);
    }
};


//偏特化 --vec
template<class T>
class LexicalCast<std::string,std::vector<T> >{
public:
    std::vector<T> operator()(const std::string& v){
            YAML::Node node = YAML::Load(v);  //string->结构
            typename std::vector<T> vec;

            std::stringstream ss;
            for(size_t i = 0;i < node.size() ; ++i){
                ss.str(""); //清空字符串流的内容 空串
                ss<<node[i];
                vec.push_back(LexicalCast<std::string,T>()(ss.str()));
            }
            return vec;
    }
};

//从vector到string
template<class T>
class LexicalCast<std::vector<T>,std::string>{
public:
    std::string operator()(const std::vector<T>& v){
            YAML::Node node;

            for(auto&i : v){
                //把他变成一个yaml节点 再把他放回去
                node.push_back(YAML::Load(LexicalCast<T,std::string>()(i)));
            }
            std::stringstream ss;
            ss<<node;
            return ss.str();
    }
};


template<class T>
class LexicalCast<std::string,std::list<T> >{
public:
    std::list<T> operator()(const std::string& v){
            YAML::Node node = YAML::Load(v);  //string->结构
            typename std::list<T> vec;

            std::stringstream ss;
            for(size_t i = 0;i < node.size() ; ++i){
                ss.str(""); //清空字符串流的内容 空串
                ss<<node[i];
                vec.push_back(LexicalCast<std::string,T>()(ss.str()));
            }
            return vec;
    }
};

//从list到string
template<class T>
class LexicalCast<std::list<T>,std::string>{
public:
    std::string operator()(const std::list<T>& v){
            YAML::Node node;

            for(auto&i : v){
                //把他变成一个yaml节点 再把他放回去
                node.push_back(YAML::Load(LexicalCast<T,std::string>()(i)));
            }
            std::stringstream ss;
            ss<<node;
            return ss.str();
    }
};

//set
template<class T>
class LexicalCast<std::string,std::set<T> >{
public:
    std::set<T> operator()(const std::string& v){
            YAML::Node node = YAML::Load(v);  //string->结构
            typename std::set<T> vec;

            std::stringstream ss;
            for(size_t i = 0;i < node.size() ; ++i){
                ss.str(""); //清空字符串流的内容 空串
                ss<<node[i];
                vec.insert(LexicalCast<std::string,T>()(ss.str()));
            }
            return vec;
    }
};

//从set到string
template<class T>
class LexicalCast<std::set<T>,std::string>{
public:
    std::string operator()(const std::set<T>& v){
            YAML::Node node;

            for(auto&i : v){
                //把他变成一个yaml节点 再把他放回去
                node.push_back(YAML::Load(LexicalCast<T,std::string>()(i)));
            }
            std::stringstream ss;
            ss<<node;
            return ss.str();
    }
};

//set
template<class T>
class LexicalCast<std::string,std::unordered_set<T> >{
public:
    std::unordered_set<T> operator()(const std::string& v){
            YAML::Node node = YAML::Load(v);  //string->结构
            typename std::unordered_set<T> vec;

            std::stringstream ss;
            for(size_t i = 0;i < node.size() ; ++i){
                ss.str(""); //清空字符串流的内容 空串
                ss<<node[i];
                vec.insert(LexicalCast<std::string,T>()(ss.str()));
            }
            return vec;
    }
};

//从unordered_set到string
template<class T>
class LexicalCast<std::unordered_set<T>,std::string>{
public:
    std::string operator()(const std::unordered_set<T>& v){
            YAML::Node node;

            for(auto&i : v){
                //把他变成一个yaml节点 再把他放回去
                node.push_back(YAML::Load(LexicalCast<T,std::string>()(i)));
            }
            std::stringstream ss;
            ss<<node;
            return ss.str();
    }
};

//map  string->map
template<class T>
class LexicalCast<std::string,std::map<std::string,T> >{
public:
    std::map<std::string,T> operator()(const std::string& v){
            YAML::Node node = YAML::Load(v);  //string->结构
            typename std::map<std::string,T> vec;

            std::stringstream ss;

            for(auto it = node.begin();it!=node.end();++it)
            {
                ss.str("");
                ss<<it->second;
                vec.insert(std::make_pair(it->first.Scalar(),
                                        LexicalCast<std::string,T>()(ss.str())));
            }
            //std::cout<<"string to map 被调用"<<"   Node is "<<node<<std::endl;
            return vec;
    }
};

//从map到string
template<class T>
class LexicalCast<std::map<std::string,T>,std::string>{
public:
    std::string operator()(const std::map<std::string,T>& v){
            YAML::Node node;

            for(auto&i : v){
                node[i.first] = YAML::Load(LexicalCast<T,std::string>()(i.second));
                
                //把他变成一个yaml节点 再把他放回去
                //node.push_back(YAML::Load(LexicalCast<T,std::string>()(i)));
            }
            std::cout<<"map to string is successful"<<std::endl;
            std::stringstream ss;
            ss<<node;
            return ss.str();
    }
};

//unordered_map
template<class T>
class LexicalCast<std::string,std::unordered_map<std::string,T> >{
public:
    std::unordered_map<std::string,T> operator()(const std::string& v){
            YAML::Node node = YAML::Load(v);  //string->结构
            typename std::unordered_map<std::string,T> vec;

            std::stringstream ss;
            for(auto it = node.begin();it!=node.end();++it)
            {
                ss.str("");
                ss<<it->second;
                vec.insert(std::make_pair(it->first.Scalar(),
                                        LexicalCast<std::string,T>()(ss.str())));
            }
            return vec;
    }
};

//从unordered_map到string
template<class T>
class LexicalCast<std::unordered_map<std::string,T>,std::string>{
public:
    std::string operator()(const std::unordered_map<std::string,T>& v){
            YAML::Node node;

            for(auto&i : v){
                node[i.first] = YAML::Load(LexicalCast<T,std::string>()(i.second));
                
                //把他变成一个yaml节点 再把他放回去
                //node.push_back(YAML::Load(LexicalCast<T,std::string>()(i)));
            }
            std::stringstream ss;
            ss<<node;
            return ss.str();
    }
};


//配置项类

//FromString T operator()(const std::string);  仿函数  通过string转成我们的类型
//ToStr std::string operator()(const T&);
template<class T,class FromStr=LexicalCast<std::string,T>
                 ,class ToStr=LexicalCast<T,std::string> > //特例化
class ConfigVar : public ConfigVarBase{  //一个具体的配置项
public:
    typedef  std::shared_ptr<ConfigVar> ptr; //shared_ptr<ConfigVar>的类型别名
    typedef std::function<void(const T&old_val,const T&new_val)> on_change_cb; //当一个配置更改的时候进行回调

    ConfigVar(const std::string& name
            ,const T& default_value
            ,const std::string& description="")
            :ConfigVarBase(name,description),m_val(default_value){  }

    std::string toString() override{ //告诉编译器这个方法是父类继承来的抽象函数，如果不是就会报错
        try{
                //boost::lexical_cast<std::string>(m_val);  //lexical_case 将成员类型转换为string类型
                return ToStr()(m_val);
        }catch(std::exception e){
            MOXU_LOG_ERROR(MOXU_LOG_ROOT())<<"ConfigVar::toString execption"<<e.what()
                    <<" convert: "<<typeid(m_val).name()<<" to string"; //typeid返回表达式的类型信息
        }
        return "";
    }

    bool fromString(const std::string& val) override{
        try{
                //std::cout<<"fromString 调用开始"<<std::endl;
               // std::cout<<val<<std::endl;
                //m_val=boost::lexical_cast<T>(val); // 将string类型转换为成员类型
                setValue(FromStr()(val));
                //std::cout<<"fromString 调用结束"<<std::endl;
        }catch(std::exception e){
            MOXU_LOG_ERROR(MOXU_LOG_ROOT())<<"ConfigVar::fromString execption"<<e.what()
                    <<" convert: string to"; //typeid返回表达式的类型信息
        }
        return false;
    }


    const T getValue() const {
        return m_val;
    }

    void setValue(const T&val){
        if(val == m_val)
            return;
    
        //回调
        for(auto& i : m_cbs){
            i.second(m_val,val);
        }
        m_val=val;
    }

    std::string getTypeName() const {
        return typeid(T).name();
    }

    //增加监听
    void addListener(uint64_t key,on_change_cb cb){
        m_cbs[key]=cb;
    }

    //删除监听
    void delListener(uint64_t key){
        m_cbs.erase(key);
    }

    on_change_cb getListener(uint64_t key){
        auto it = m_cbs.find(key);
        return it == m_cbs.end() ? nullptr : it;
    }

private:
    T m_val;
    //变更回调函数组     为什么用Map因为function没有比较函数，所以我们在将他放在vector中的话无法判断是否相等。
    //uin64_t key 要求为1,一般用Hash
    std::map<uint64_t ,on_change_cb> m_cbs;
};

//配置管理类  -- 配置项的注册，查找，加载......
class Config{
public:
    typedef std::map<std::string,ConfigVarBase::ptr> ConfigVarMap;   
    template<class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string & name,
        const T&default_value,const std::string description=""){
        std::cout<<"The s_datas value commit is next in"<<__func__<<std::endl;

  typename ConfigVar<int>::ptr iv(new ConfigVar<int>("int",4,"int num"));

        s_datas["int"] = iv;

std::cout<<"no error in"<<__func__<<"the num is "<<__LINE__<<std::endl;

 std::cout<<"the "<<__func__<<" is running......"<<" The line is "<<__LINE__<<std::endl;
            
            std::cout<<"The s_datas大小is "<<s_datas.size()<<"in the fun "<<__func__<<" \n";

            auto it = s_datas.find(name); //返回迭代器
            if(it != s_datas.end()){
                auto tmp = std::dynamic_pointer_cast<ConfigVar<T> >(it->second);
                if(tmp){
                    //已经存在了
                     MOXU_LOG_INFO(MOXU_LOG_ROOT()) << "Lookup name="<<name<<"exists";
                     std::cout<<"存在"<<std::endl;
                    return tmp;
                }else{
                    MOXU_LOG_ERROR(MOXU_LOG_ROOT()) << "Lookup name="<<name<<"exists but type not " 
                        <<typeid(T).name() << " real_type "<<it->second->getTypeName()
                        <<" "<<it->second->toString();
                    return nullptr;
                }
            }
            std::cout<<"no error in"<<__func__<<" after the (not in s_datas.end()) the num is "<<__LINE__<<std::endl;
            if(name.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ._0123456789")
                !=std::string::npos)
                {
                    MOXU_LOG_ERROR(MOXU_LOG_ROOT()) << "Lookup name invalid "<<name;
                    throw std::invalid_argument(name); //输入的参数值无效
                }
                typename ConfigVar<T>::ptr v(new ConfigVar<T>(name,default_value,description));
                 std::cout<<"The s_datas大小is "<<s_datas.size()<<" in the fun "<<__func__<<" \n";
            
                s_datas[name]=v;
               // std::cout<<"插入"<<std::endl;
                //std::cout<<"no error in end of "<<__func__<<" The v is  "<<v-><<std::endl;
                return v;
        }



    template<class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string name){ //查找
        auto it = s_datas.find(name);
        if(it->second){
            return nullptr;
        }
        return std::dynamic_pointer_cast<ConfigVar<T>>(it->second); //dynamic_pointer_cast()将/一个派生类智能ptr转换为基类ptr
    }

    static void LoadFromYaml(const YAML::Node& root);

    static ConfigVarBase::ptr LookupBase(const std::string& name); //查找有没有当前的项 如果有的话返回
private:
    static ConfigVarMap s_datas;
};

}

#endif