#ifndef CRYPTO_X_H
#define CRYPTO_X_H
#include <cstdlib>
#include <ctime>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <cstring>
#include <openssl/sha.h>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

class CryptoX {
public:
    CryptoX();
    std::string generateIV();
    std::vector<unsigned char> encrypt(const std::string& plaintext, const std::string& key, const std::string& iv);
    std::string decrypt(const std::vector<unsigned char>& ciphertext, const std::string& key, const std::string& iv);
    std::string generateCompatibleKey(const std::string& keyStr);
std::string unpad_string(const std::string& str);
private:
    
};

#endif // CRYPTO_X_H
