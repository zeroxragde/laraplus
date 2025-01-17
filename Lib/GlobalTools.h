#ifndef GLOBALTOOLS_H
#define GLOBALTOOLS_H

#include <string>

class GlobalTools {
public:
    static std::string generateSalt(size_t length);
    static std::string sha256(const std::string& input);
    static std::string getCurrentDateTime();
    static bool esJsonValido(const std::string& str);
    static void lomPrint(const std::string& message); // Declarar lomPrint
   
};

#endif // GLOBALTOOLS_H
