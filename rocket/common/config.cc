#include "/home/xxxj/rpc/rocket/rocket/common/config.h"
#include<tinyxml/tinyxml.h>
#include<stdio.h>

#define READ_XML_NODE(name,parent)                                         \
TiXmlElement* name##_node = parent->FirstChildElement(#name);              \
if(!name##_node){                                                          \
    printf("Start rocket server error,failed to read node [%s]\n",#name);  \
    exit(0);                                                               \
}

#define  READ_STR_FROM_XML_NODE(name,parent) \
TiXmlElement* name##_node = parent->FirstChildElement(#name); \
    if(!name##_node || !name##_node->GetText()){         \
        printf("Start rocket server error,failed to read node [%s]\n",#name); \
        exit(0); \
    }\
    std::string name##_str = std::string(name##_node->GetText()); \


namespace rocket{

static Config* g_config = NULL;

Config* Config::GetGlobalConfiger(){
    return g_config;
}
void Config::SetGlobalConfiger(const char* xmlfile){
    if(g_config == NULL){
        g_config = new Config(xmlfile); //传一个xml文件
    }
}


Config::Config(const char* xmfile){
    TiXmlDocument* xml_document = new TiXmlDocument();
    // TiXmlNode root_node = 
    // xml_document->FirstChildElement();
    bool rt = xml_document->LoadFile(xmfile);
    if(!rt){
        printf("Start rocket server error,failed to read config file %s",xmfile);
        // xml_document
        exit(0);
    }

    READ_XML_NODE(root,xml_document); //1
    READ_XML_NODE(log,root_node);   //2
    READ_STR_FROM_XML_NODE(log_level,log_node); //3
    
    m_log_level = log_level_str;
    //1
    // TiXmlNode* root_node = xml_document->FirstChildElement("root");
    // if(!root_node){
    //     printf("Start rocket server error,failed to read node [%s]\n","root");
    //     exit(0);
    // }
    //2
    // TiXmlElement* log_node = root_node->FirstChildElement("log");
    // if(!log_node){
    //     printf("Start rocket server error,failed to read node [%s]\n","log");
    //     exit(0);
    // }
    //3
    // TiXmlElement* log_level_node = log_node->FirstChildElement("log_level");
    // if(!log_level_node || !log_level_node->GetText()){
    //     printf("Start rocket server error,failed to read node [%s]\n","log_level");
    //     exit(0);
    // }
    // std::string log_level = std::string(log_level_node->GetText());
}

};