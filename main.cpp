#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <functional>

// Funciones para manejar los comandos
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <filesystem> // Para manejar directorios (C++17)--



void makeModal(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Error: Debes proporcionar el nombre del modal.\n";
        return;
    }

    std::string modalName = args[0];

    // Convertir el nombre del modal a PascalCase
    modalName[0] = toupper(modalName[0]);
    for (size_t i = 1; i < modalName.size(); ++i) {
        modalName[i] = tolower(modalName[i]);
    }

    // Crear directorio si no existe
    std::string directory = "Modelos";
    if (!std::filesystem::exists(directory)) {
        std::filesystem::create_directories(directory);
        std::cout << "Directorio creado: " << directory << std::endl;
    }

    // Crear el archivo .h
    std::string headerFileName = directory + "/" + modalName + ".h";
    std::ofstream headerFile(headerFileName);
    if (headerFile.is_open()) {
        headerFile << "#ifndef " << modalName << "_H\n";
        headerFile << "#define " << modalName << "_H\n\n";
        headerFile << "#include \"../Lib/Modelo.h\"\n\n";
        headerFile << "class " << modalName << " : public Modelo {\n";
        headerFile << "public:\n";
        headerFile << "    " << modalName << "();\n";
        headerFile << "    static void register" << modalName << "() {\n";
        headerFile << "        Modelo::registerClass(\"" << modalName << "\", []() { return std::make_unique<" << modalName << ">(); });\n";
        headerFile << "    }\n";
        headerFile << "};\n\n";
        headerFile << "#endif // " << modalName << "_H\n";
        headerFile.close();
        std::cout << "Archivo " << headerFileName << " creado.\n";
    } else {
        std::cerr << "Error al crear el archivo " << headerFileName << ".\n";
        return;
    }

    // Crear el archivo .cpp
    std::string cppFileName = directory + "/" + modalName + ".cpp";
    std::ofstream cppFile(cppFileName);
    if (cppFile.is_open()) {
        cppFile << "#include \"" << modalName << ".h\"\n\n";
        cppFile << modalName << "::" << modalName << "() : Modelo(\"" << modalName << "s\", \"" << modalName << "_id\") {}\n";
        cppFile.close();
        std::cout << "Archivo " << cppFileName << " creado.\n";
    } else {
        std::cerr << "Error al crear el archivo " << cppFileName << ".\n";
    }
}


void makeMigration(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Error: Debes proporcionar el nombre de la tabla.\n";
        return;
    }

    std::string tableName = args[0];
    std::string migrationName = "create_" + tableName + "_table";

    // Crear directorio si no existe
    std::string directory = "database/migrations";
    if (!std::filesystem::exists(directory)) {
        std::filesystem::create_directories(directory);
        std::cout << "Directorio creado: " << directory << std::endl;
    }

    // Crear archivo de migración
    std::string timestamp = std::to_string(std::time(nullptr)); // Usar marca de tiempo única
    std::string fileName = directory + "/" + timestamp + "_" + migrationName + ".cpp";

    std::ofstream migrationFile(fileName);
    if (migrationFile.is_open()) {
        migrationFile << "#include \"../Libs/Schema.h\"\n\n";
        migrationFile << "class " << migrationName << " : public Schema {\n";
        migrationFile << "public:\n";
        migrationFile << "    void up() override {\n";
        migrationFile << "        createTable(\"" << tableName << "\", {\n";
        migrationFile << "            {\"id\", \"INT AUTO_INCREMENT PRIMARY KEY\"},\n";
        migrationFile << "            {\"created_at\", \"DATETIME\"},\n";
        migrationFile << "            {\"updated_at\", \"DATETIME\"}\n";
        migrationFile << "        });\n";
        migrationFile << "    }\n\n";
        migrationFile << "    void down() override {\n";
        migrationFile << "        dropTable(\"" << tableName << "\");\n";
        migrationFile << "    }\n";
        migrationFile << "};\n";

        migrationFile.close();
        std::cout << "Archivo de migración creado: " << fileName << std::endl;
    } else {
        std::cerr << "Error al crear el archivo de migración " << fileName << ".\n";
    }
}
void migrate(const std::vector<std::string>& args) {
    const std::string migrationsPath = "database/migrations";

    if (!fs::exists(migrationsPath)) {
        std::cerr << "Error: La carpeta de migraciones no existe en " << migrationsPath << "\n";
        return;
    }

    for (const auto& entry : fs::directory_iterator(migrationsPath)) {
        if (entry.path().extension() == ".so") { // Buscando archivos compilados como .so
            std::string migrationPath = entry.path().string();
            void* handle = dlopen(migrationPath.c_str(), RTLD_LAZY);
            if (!handle) {
                std::cerr << "Error cargando migración: " << dlerror() << "\n";
                continue;
            }

            // Buscar la función `createMigration`
            typedef Schema* (*CreateMigration)();
            CreateMigration createMigration = (CreateMigration)dlsym(handle, "createMigration");
            if (!createMigration) {
                std::cerr << "Error encontrando la función createMigration en " << migrationPath << "\n";
                dlclose(handle);
                continue;
            }

            // Crear y ejecutar la migración
            Schema* migration = createMigration();
            std::cout << "Ejecutando migración: " << entry.path().filename() << "\n";
            migration->up();
            delete migration;

            dlclose(handle);
        }
    }

    std::cout << "Todas las migraciones ejecutadas correctamente.\n";
}
void makeController(const std::vector<std::string>& args) {
    if (args.size() < 1) {
        std::cerr << "Error: Falta el nombre del controlador.\n";
        return;
    }
    std::cout << "Creando controlador con nombre: " << args[0] << std::endl;
}

// Clase para manejar los comandos
class CommandHandler {
public:
    using CommandFunction = std::function<void(const std::vector<std::string>&)>;

    void registerCommand(const std::string& command, CommandFunction func) {
        commands[command] = func;
    }

    void executeCommand(int argc, char* argv[]) {
        if (argc < 2) {
            std::cerr << "Error: No se proporcionó ningún comando.\n";
            printHelp();
            return;
        }

        std::string command = argv[1];
        std::vector<std::string> args(argv + 2, argv + argc);

        if (commands.find(command) != commands.end()) {
            commands[command](args);
        } else {
            std::cerr << "Error: Comando no reconocido: " << command << "\n";
            printHelp();
        }
    }

    void printHelp() const {
        std::cout << "Comandos disponibles:\n";
        for (const auto& cmd : commands) {
            std::cout << "  - " << cmd.first << std::endl;
        }
    }

private:
    std::map<std::string, CommandFunction> commands;
};

// Función principal
int main(int argc, char* argv[]) {
    CommandHandler handler;

    // Registrar comandos
    handler.registerCommand("make:modal", makeModal);
    handler.registerCommand("make:migration", makeMigration);
    handler.registerCommand("make:controller", makeController);
    handler.registerCommand("migrate", migrate);
    // Ejecutar el comando
    handler.executeCommand(argc, argv);

    return 0;
}
