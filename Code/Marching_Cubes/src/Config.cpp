#include "Config.h"
nlohmann::json Config::configJson;
std::unordered_map<std::string,bool> Config::toggles;
bool Config::isInit = false;

void Config::initConfig()
{
    if (isInit) return;
    std::ifstream configFile("config.json");
    configFile >> configJson;
    isInit = true;
}


glm::vec3 Config::getVec3(std::string vecName)
{
    std::vector<float> arr = configJson[vecName];
    return glm::vec3(arr[0],arr[1],arr[2]);
}

btVector3 Config::getBtVec3(std::string vecName)
{
    std::vector<float> arr = configJson[vecName];
    return btVector3(arr[0],arr[1],arr[2]);
}




void Config::setToggle(std::string configName, bool value) {
    toggles[configName] = value;
}