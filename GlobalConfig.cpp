#include "GlobalConfig.h"

GlobalConfig& GlobalConfig::getInstance() {
    static GlobalConfig instance;
    return instance;
}

bool GlobalConfig::loadConfig(const std::string& configFilePath) {
    std::ifstream configFile(configFilePath);
    if (!configFile.is_open()) {
        std::cerr << "Error al abrir el archivo de configuración." << std::endl;
        return false;
    }

    json jsonConfig;
    configFile >> jsonConfig;
    configFile.close();

    mysqlConfig.host = jsonConfig["mysql"]["host"];
    mysqlConfig.user = jsonConfig["mysql"]["user"];
    mysqlConfig.password = jsonConfig["mysql"]["password"];
    mysqlConfig.database = jsonConfig["mysql"]["database"];

    Admin admin;
    admin.pass = jsonConfig["web"]["admin"]["pass"];
    admin.user = jsonConfig["web"]["admin"]["user"];
    webConn.admin=admin;

    encryptionConfig.password = jsonConfig["encryption"]["password"];

    gameServerPort = jsonConfig.value("gameServerPort", "");
    webServerPort = jsonConfig.value("webServerPort", "");
    salt = jsonConfig.value("salt", "");

    return true;
}

bool GlobalConfig::saveConfig(const std::string& configFilePath) {
    std::ofstream configFile(configFilePath);
    if (!configFile.is_open()) {
        std::cerr << "Error al abrir el archivo de configuración para escritura." << std::endl;
        return false;
    }

    json jsonConfig;
    jsonConfig["mysql"] = {
        {"host", mysqlConfig.host},
        {"user", mysqlConfig.user},
        {"password", mysqlConfig.password},
        {"database", mysqlConfig.database}
    };
    jsonConfig["web"]["admin"] = {
        {"user", webConn.admin.user},
        {"pass", webConn.admin.pass},
    };



    jsonConfig["encryption"] = {{"password", encryptionConfig.password}};
    jsonConfig["gameServerPort"] = gameServerPort;
    jsonConfig["webServerPort"] = webServerPort;
    jsonConfig["salt"] = salt;

    configFile << jsonConfig.dump(4);
    configFile.close();

    return true;
}

void GlobalConfig::setMySQLConfig(const MySQLConfig& config) {
    mysqlConfig = config;
}
void GlobalConfig::setWebConfig(const WebConfig& config) {
    webConn = config;
}

void GlobalConfig::setEncryptionConfig(const EncryptionConfig& config) {
    encryptionConfig = config;
}

void GlobalConfig::setGameServerPort(const std::string& port) {
    gameServerPort = port;
}

void GlobalConfig::setWebServerPort(const std::string& port) {
    webServerPort = port;
}

void GlobalConfig::setSalt(const std::string& salt) {

    this->salt = salt;
}

MySQLConfig GlobalConfig::getMySQLConfig() const {
    return mysqlConfig;
}

EncryptionConfig GlobalConfig::getEncryptionConfig() const {
    return encryptionConfig;
}

std::string GlobalConfig::getGameServerPort() {
    return gameServerPort;
}
WebConfig GlobalConfig::getWebConn() {
    return webConn;
}

std::string GlobalConfig::getWebServerPort() {
    return webServerPort;
}
std::string GlobalConfig::getSalt() {
    return salt;
}
