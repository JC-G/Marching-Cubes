#include "Config.h"
nlohmann::json Config::configJson;
bool Config::isInit = false;
bool Config::wireframe = false;

void Config::initConfig()
{
    if (isInit) return;
    std::ifstream configFile("config.json");
    configFile >> configJson;
    isInit = true;

    Config::wireframe = get<bool>("wireframe");
}


glm::vec3 Config::getVec3(std::string vecName)
{
    std::vector<float> arr = configJson[vecName];
    return glm::vec3(arr[0],arr[1],arr[2]);
}




