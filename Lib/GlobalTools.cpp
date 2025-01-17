#include "GlobalTools.h"
#include <openssl/sha.h>
#include <random>
#include <sstream>
#include <iomanip>
#include "../include/json.hpp"
#include <iostream>
//OBTENER FECHA
std::string GlobalTools::getCurrentDateTime() {

  // Obtener la hora actual

  std::time_t now = std::time(nullptr);

  // Convertir la hora actual a una estructura tm

  std::tm * localTime = std::localtime( & now);

  // Usar un stringstream para formatear la fecha y hora

  std::ostringstream oss;

  oss << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");

  // Devolver la cadena formateada
  return oss.str();
}
// Implementación de generateSalt
std::string GlobalTools::generateSalt(size_t length) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::default_random_engine rng(std::random_device{}());
    std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);

    std::string salt;
    for (size_t i = 0; i < length; ++i) {
        salt += charset[dist(rng)];
    }
    return salt;
}
bool GlobalTools::esJsonValido(const std::string& str) {
    try {
        // Intentar parsear el string como JSON
        nlohmann::json::parse(str);
        return true; // Si no se produce una excepción, es un JSON válido
    } catch (nlohmann::json::parse_error& e) {
        return false; // Si se produce una excepción, no es un JSON válido
    }
}
// Implementación de sha256
std::string GlobalTools::sha256(const std::string& input) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, input.c_str(), input.size());
    SHA256_Final(hash, &sha256);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}
void GlobalTools::lomPrint(const std::string& message) {
   std::cout << "PETS> " << message << std::endl;
}