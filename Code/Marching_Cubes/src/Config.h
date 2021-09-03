#ifndef CONFIG_H
#define CONFIG_H


#include "json.hpp"
#include "GL_headers.h"
#include <fstream>
#include <string>
#include <iostream>
class Config
{
    public:
        static void initConfig();

        template<class T> static T get(std::string configName);
        static glm::vec3 getVec3(std::string vecName);
    protected:

    private:
        static bool isInit;
        static nlohmann::json configJson;
};

template<class T> T Config::get(std::string configName)
{
    if (!isInit) {
        initConfig();
    }
    T result = configJson[configName];
    return result;
}

#endif // CONFIG_H
