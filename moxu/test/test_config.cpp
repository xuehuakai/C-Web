#include"../src/config.h"
#include"../src/log.h"
#include<yaml-cpp/yaml.h>


moxu::ConfigVar<int>::ptr g_int_value_config = 
        moxu::Config::Lookup("system.port",(int)8080,"system port");
#if 0
moxu::ConfigVar<float>::ptr g_ints_value_config = 
        moxu::Config::Lookup("system.port",(float)8080,"system port");  /*报ERROR的前提是前面已经有一个正确的name
        且是一样的，这样在lookup的时候才不会创建而是直接返回之前的，所以在find的时候强转的时候就会失败，就会报ERROR*/


moxu::ConfigVar<float>::ptr g_float_value_config=
        moxu::Config::Lookup("system.value",(float)10.2f,"system value");

moxu::ConfigVar<std::vector<int> >::ptr g_int_vec_value_config=
        moxu::Config::Lookup("system.int_vec",std::vector<int>{1,2,4,5,6,},"system int vec");
moxu::ConfigVar<std::list<int> >::ptr g_int_list_value_config=
        moxu::Config::Lookup("system.int_list",std::list<int>{32,42},"system int list");

moxu::ConfigVar<std::set<int> >::ptr g_int_set_value_config=
        moxu::Config::Lookup("system.int_set",std::set<int>{32000,42000,21,2,1},"system int set");

moxu::ConfigVar<std::unordered_set<int> >::ptr g_int_unordered_set_value_config=
        moxu::Config::Lookup("system.int_unordered_set",std::unordered_set<int>{100,32,42000,324,1},"\
        system int unordered_set");

moxu::ConfigVar<std::map<std::string,int> >::ptr g_str_int_map_value_config=
        moxu::Config::Lookup("system.str_int_map",std::map<std::string,int>{{"k",2},{"a",4}},"system str int map");

moxu::ConfigVar<std::unordered_map<std::string,int> >::ptr g_str_int_umap_value_config=
        moxu::Config::Lookup("system.str_int_umap",std::unordered_map<std::string,int>{{"k",2000000},{"a",40000}},"system str int umap");

void print_yaml(const YAML::Node &node,int level){
    if(node.IsScalar()){ //如果是标量节点：字符串，数字，布尔值.
        MOXU_LOG_INFO(MOXU_LOG_ROOT()) <<std::string(level*4,' ')
            <<node.Scalar()<<" - "<<node.Type()<<" - "<<level<<" Scalar";
    }else if(node.IsNull()){
        MOXU_LOG_INFO(MOXU_LOG_ROOT()) <<std::string(level*4,' ')
            <<"NULL - "<<" - "<<node.Type()<<level;
    }else if (node.IsMap()){
        for(auto it = node.begin();
                it!=node.end();it++){
            MOXU_LOG_INFO(MOXU_LOG_ROOT()) <<std::string(level*4,' ')
                <<it->first<<" - "<<it->second.Type()<<" - "<<level<<" Map";
            print_yaml(it->second,level+1);
        }
    }else if(node.IsSequence()){
        for(size_t i = 0;i<node.size();i++){
            MOXU_LOG_INFO(MOXU_LOG_ROOT())<<std::string(level*4,' ')
                <<i<<" - "<<node[i].Type()<<" - "<<level<<" "<<"Sequence";
            print_yaml(node[i],level+1);
        }
        }
}

void test_ymal(){
    YAML::Node root = YAML::LoadFile("/a/workspace/moxu/bin/conf/test.yml");
    std::cout<<"no error in"<<__func__<<"the num is "<<__LINE__<<std::endl;
    print_yaml(root,0);
    std::cout<<"no error in"<<__func__<<"the num is "<<__LINE__<<std::endl;
    MOXU_LOG_INFO(MOXU_LOG_ROOT()) <<root.Scalar();  //将文件信息输出到控制台中
}

void test_config(){
    MOXU_LOG_INFO(MOXU_LOG_ROOT())<<"before: "<<g_int_value_config->getValue();
    MOXU_LOG_INFO(MOXU_LOG_ROOT())<<"before: "<<g_float_value_config->toString();

    #define XX(g_var,name,prefix) \
    {   \
        auto &v = g_var->getValue(); \
        for(auto & i: v)  {  \
            MOXU_LOG_INFO(MOXU_LOG_ROOT())<<#prefix" " #name ": "<<i; \
        } \
        MOXU_LOG_INFO(MOXU_LOG_ROOT())<<#prefix" " #name "yaml: "<<g_var->toString();\
    } 

    #define XX_M(g_var,name,prefix) \
    {   \
        auto &v = g_var->getValue(); \
        for(auto & i: v)  {  \
            MOXU_LOG_INFO(MOXU_LOG_ROOT())<<#prefix" " #name ": {" \
            <<i.first<<" - " <<i.second << "}"; \
        } \
        MOXU_LOG_INFO(MOXU_LOG_ROOT())<<#prefix" " #name "yaml: "<<g_var->toString();\
    } 


    XX(g_int_vec_value_config,int_vec,before);
    XX(g_int_list_value_config,int_list,before);
    XX(g_int_set_value_config,int_set,before);
    XX(g_int_unordered_set_value_config,int_unordered_set,before);
    XX_M(g_str_int_map_value_config,str_int_map,before);
    XX_M(g_str_int_umap_value_config,s_str_int_ump,before);

    YAML::Node root = YAML::LoadFile("/a/workspace/moxu/bin/conf/test.yml");
    moxu::Config::LoadFromYaml(root);  //将解析的结果存进map中

    MOXU_LOG_INFO(MOXU_LOG_ROOT())<<"after: "<<g_int_value_config->getValue();
    MOXU_LOG_INFO(MOXU_LOG_ROOT())<<"after: "<<g_float_value_config->toString();
    
    XX(g_int_vec_value_config,int_vec,after);
    XX(g_int_list_value_config,int_list,after);
    XX(g_int_set_value_config,int_set,after);
    XX(g_int_unordered_set_value_config,int_unordered_set,after);
    XX_M(g_str_int_map_value_config,str_int_map,after);
    XX_M(g_str_int_umap_value_config,s_str_int_ump,after);
}

class Person{
public:
    Person(){

    }
    std::string m_name;
    int m_age = 0;
    bool m_sex = 0;

    std::string toString() const {
        std::stringstream ss;
        ss<<"{Person name="<<m_name
          <<" age="<<m_age
          <<" sex="<<(m_sex?"男":"女")
          <<"}";
        return ss.str();
    }

    bool operator==(const Person& ot) const{
        return (ot.m_age == m_age) && (ot.m_name == m_name) && (ot.m_sex == m_sex);
    }

};


namespace moxu{
    template<>
    class LexicalCast<std::string, Person>{
        public:
            Person operator()(const std::string & v){
                YAML::Node node = YAML::Load(v);
                Person p;
                p.m_name = node["name"].as<std::string>();
                p.m_age =node["age"].as<int>();
                p.m_sex = node["sex"].as<bool>();
                return p;
            }
    };
 
    template<>
    class LexicalCast<Person,std::string>{
        public:
            std::string operator()(const Person& v){

                YAML::Node node;
                node["name"]=v.m_name;
                node["age"]=v.m_age;
                node["sex"]=v.m_sex;

                std::stringstream ss;
                ss<<node;
                return ss.str();
            }
           

    };
}

moxu::ConfigVar<Person>::ptr g_person = 
        moxu::Config::Lookup("class.person",Person(),"system person");

moxu::ConfigVar<std::map<std::string,Person> >::ptr g_person_map = 
        moxu::Config::Lookup("class.map",std::map<std::string,Person>{{"dad",Person()}},"system map");

// moxu::ConfigVar<std::map<std::string,Person> >::ptr g_person_vec_map = 
//         moxu::Config::Lookup("class.vec_map",std::map<std::string,std::vector<Person> >{{"dad",{Person(),Person()}}},"system vec map");

moxu::ConfigVar<std::map<std::string, std::vector<Person> > >::ptr g_person_vec_map =
    moxu::Config::Lookup("class.vec_map",
                         std::map<std::string, std::vector<Person> >{{"dad",{Person(),Person()}}},
                         "system vec map");


//自定义类
void test_class(){


#define XX_PM(g_person_map,prifx) { \
    auto m = g_person_map->getValue(); \
    for(auto& i : m){ \
        MOXU_LOG_INFO(MOXU_LOG_ROOT())<<#prifx<<": "<<i.first<<" - "<<i.second.toString(); \
    } \
}

    g_person->addListener(10,[](const Person& old_val,const Person& new_vale){
            MOXU_LOG_INFO(MOXU_LOG_ROOT())<<"old_val = "<<old_val.toString() 
                                          <<"new_val = "<<new_vale.toString();  
    });

    MOXU_LOG_INFO(MOXU_LOG_ROOT()) <<"before: "<<g_person_vec_map->toString();
    // MOXU_LOG_INFO(MOXU_LOG_ROOT())<<"before: "<<g_person->getValue().toString()<<" - "<<g_person->toString();
    //  XX_PM(g_person_map,class.map before);
    YAML::Node root = YAML::LoadFile("/a/workspace/moxu/bin/conf/test.yml");
    moxu::Config::LoadFromYaml(root);  //将解析的结果存进map中
    // MOXU_LOG_INFO(MOXU_LOG_ROOT())<<"after: "<<g_person->getValue().toString()<<" - "<<g_person->toString();
    //  XX_PM(g_person_map,class.map after);
    //  //std::cout<<"map测试结束"<<std::endl;
     MOXU_LOG_INFO(MOXU_LOG_ROOT()) <<"after "<<g_person_vec_map->toString();
}
#endif

void test_log(){
     YAML::Node root = YAML::LoadFile("/a/workspace/moxu/bin/conf/log.yml");

    moxu::Config::LoadFromYaml(root);

}

int main(){
    std::cout<<"这是test_config"<<std::endl;
    //test_log();
   // test_class();
  // test_ymal();
    test_log();
    std::cout<<"main fun is end!"<<std::endl;
    return 0;
}