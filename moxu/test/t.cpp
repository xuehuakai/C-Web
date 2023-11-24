#include<iostream>

#include"../src/config.h"
#include"../src/log.h"


void test_log(){
     YAML::Node root = YAML::LoadFile("/a/workspace/moxu/bin/conf/log.yml");

    moxu::Config::LoadFromYaml(root);

}

int main(){
          std::cout<<"t文件"<<std::endl;
		return 0;
}
