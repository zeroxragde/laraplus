#include "crypto.h"

// Definir AES_BLOCK_SIZE si no está definido
#ifndef AES_BLOCK_SIZE
#define AES_BLOCK_SIZE 16  // AES utiliza un bloque de 16 bytes (128 bits)
#endif

CryptoX::CryptoX() {}

std::string CryptoX::generateIV() {
    std::string iv(16, '\0'); // Inicializa un string de 16 bytes
    RAND_bytes(reinterpret_cast<unsigned char*>(&iv[0]), iv.size()); // Genera bytes aleatorios
    return iv;
}

std::string CryptoX::generateCompatibleKey(const std::string& keyStr) {
   unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(keyStr.c_str()), keyStr.size(), hash);

    std::string key;
    for (int i = 0; i < 16; ++i) { // Tomar solo los primeros 16 bytes
        char c = static_cast<char>(hash[i] % 92 + 33); // Generar un carácter imprimible
        // Asegurarse de que no sea / o 
        if (c != '/' && c != '\\') {
            key += c;
        } else {
            // Si es / o \, elige un carácter alternativo
            key += '!';
        }
    }
    return key;
}

std::vector<unsigned char> CryptoX::encrypt(const std::string& plaintext, const std::string& key, const std::string& iv) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, reinterpret_cast<const unsigned char*>(key.c_str()), reinterpret_cast<const unsigned char*>(iv.c_str()));

    std::vector<unsigned char> ciphertext(plaintext.size() + EVP_MAX_BLOCK_LENGTH);
    int numBytesEncrypted = 0;

    EVP_EncryptUpdate(ctx, ciphertext.data(), &numBytesEncrypted, reinterpret_cast<const unsigned char*>(plaintext.c_str()), plaintext.size());
    int finalBytesEncrypted = 0;
    EVP_EncryptFinal_ex(ctx, ciphertext.data() + numBytesEncrypted, &finalBytesEncrypted);

    ciphertext.resize(numBytesEncrypted + finalBytesEncrypted);
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext;
}

std::string CryptoX::decrypt(const std::vector<unsigned char>& ciphertext, const std::string& key, const std::string& iv) {
 
 
     if (key.size() != 16 || iv.size() != 16) {
        throw std::invalid_argument("Key and IV must be 16 bytes long.");
    }

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        throw std::runtime_error("Failed to create cipher context.");
    }

    if (EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, reinterpret_cast<const unsigned char*>(key.c_str()), reinterpret_cast<const unsigned char*>(iv.c_str())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize decryption.");
    }

    std::vector<unsigned char> decrypted(ciphertext.size() + AES_BLOCK_SIZE);
    int decrypted_len = 0;
    int final_len = 0;

    // Desencriptar los datos
    if (EVP_DecryptUpdate(ctx, decrypted.data(), &decrypted_len, ciphertext.data(), static_cast<int>(ciphertext.size())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to decrypt data.");
    }

    // Finaliza la desencriptación
    if (EVP_DecryptFinal_ex(ctx, decrypted.data() + decrypted_len, &final_len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Failed to finalize decryption.");
    }

    EVP_CIPHER_CTX_free(ctx);
    
    // Redimensionar el vector para incluir los bytes finales
    decrypted.resize(decrypted_len + final_len);
    
    // Convertir el vector a string
    std::string decrypted_text(reinterpret_cast<char*>(decrypted.data()), decrypted.size());
    
    // Eliminar el padding
    return unpad_string(decrypted_text);

}

// Función para remover el padding de la cadena desencriptada
std::string CryptoX::unpad_string(const std::string& str) {
       size_t padding = static_cast<size_t>(str.back());
    if (padding > 0 && padding <= AES_BLOCK_SIZE) { // Asegúrate de incluir openssl/aes.h para AES_BLOCK_SIZE
        return str.substr(0, str.size() - padding);
    }
    return str;
}