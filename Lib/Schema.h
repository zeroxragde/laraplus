#ifndef SCHEMA_H
#define SCHEMA_H

#include <string>
#include <vector>
#include <map>
#include "dbController.h"

class Schema {
public:
    Schema();
    virtual ~Schema();

    // Métodos para definir la estructura de las migraciones
    virtual void up() = 0; // Definición de la migración (crear)
    virtual void down() = 0; // Reversión de la migración (eliminar)

    // Métodos para operaciones de tabla
    bool createTable(const std::string& tableName, const std::vector<std::map<std::string, std::string>>& fields);
    bool dropTable(const std::string& tableName);
    bool addColumn(const std::string& tableName, const std::string& columnName, const std::string& columnType);
    bool dropColumn(const std::string& tableName, const std::string& columnName);

protected:
    dbController _db; // Controlador de la base de datos
};

#endif // SCHEMA_H
