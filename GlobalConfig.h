#ifndef GLOBAL_CONFIG_H
#define GLOBAL_CONFIG_H

#include <fstream>
#include <iostream>
#include "include/json.hpp"

using json = nlohmann::json;

// Estructura para almacenar la configuración de MySQL
struct MySQLConfig {
    std::string host;
    std::string user;
    std::string password;
    std::string database;
};
struct Admin{
  std::string user;
  std::string pass;
};
// Estructura de web
struct WebConfig{
  Admin admin;
};

// Estructura para almacenar la configuración de Encryption
struct EncryptionConfig {
    std::string password;
};

// Clase Singleton para manejar la configuración global
class GlobalConfig {
public:
    // Obtener la instancia única del singleton
    static GlobalConfig& getInstance();

    // Cargar la configuración desde un archivo JSON
    bool loadConfig(const std::string& configFilePath);
    
    // Guardar la configuración en un archivo JSON
    bool saveConfig(const std::string& configFilePath);

    // Métodos para establecer valores de configuración
    void setMySQLConfig(const MySQLConfig& config);
    void setWebConfig(const WebConfig& config);
    void setEncryptionConfig(const EncryptionConfig& config);
    void setGameServerPort(const std::string& port);
    void setWebServerPort(const std::string& port);
    void setSalt(const std::string& salt);

    // Obtener la configuración de MySQL
    MySQLConfig getMySQLConfig() const;

    // Obtener la configuración de Encryption
    EncryptionConfig getEncryptionConfig() const;

    WebConfig getWebConn(); 
    
    std::string getGameServerPort(); 
    std::string getWebServerPort(); 
    std::string getSalt(); 

private:
    // Constructor privado para el patrón singleton
    GlobalConfig() {}

    // Almacenar la configuración de MySQL
    MySQLConfig mysqlConfig;

    // Almacear la configuracion web
    WebConfig webConn;

    // Almacenar la configuración de Encryption
    EncryptionConfig encryptionConfig;

    // Almacenar los puertos y la clave salt
    std::string gameServerPort;
    std::string webServerPort;
    std::string salt;

    // No permitir la copia o asignación del singleton
    GlobalConfig(const GlobalConfig&) = delete;
    GlobalConfig& operator=(const GlobalConfig&) = delete;
};

#endif // GLOBAL_CONFIG_H
