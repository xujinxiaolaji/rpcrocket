#ifndef ROCKET_COMMON_CONFIG_H
#define ROCKET_COMMON_CONFIG_H

#include<map>
#include<string>

namespace rocket{

class Config{
public:
    Config(const char* xmfile);

public:
    static Config* GetGlobalConfiger();
    static void SetGlobalConfiger(const char* xmlfile);

public:
    // std::map<std::string,std::string> m_config_values;
    std::string m_log_level;
};


};


#endif