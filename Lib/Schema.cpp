#include "Schema.h"
#include <iostream>

// Constructor
Schema::Schema() : _db() {}

// Destructor
Schema::~Schema() {}

// Crear tabla
bool Schema::createTable(const std::string& tableName, const std::vector<std::map<std::string, std::string>>& fields) {
    if (_db.existe_tabla(tableName)) {
        std::cerr << "Error: La tabla '" << tableName << "' ya existe.\n";
        return false;
    }

    std::string query = "CREATE TABLE " + tableName + " (";
    for (size_t i = 0; i < fields.size(); ++i) {
        const auto& field = fields[i];
        query += field.at("name") + " " + field.at("type");
        if (field.count("constraints")) {
            query += " " + field.at("constraints");
        }
        if (i < fields.size() - 1) {
            query += ", ";
        }
    }
    query += ");";

    return _db.insertar(query, {}); // Método de `dbController` para ejecutar consultas
}

// Eliminar tabla
bool Schema::dropTable(const std::string& tableName) {
    if (!_db.existe_tabla(tableName)) {
        std::cerr << "Error: La tabla '" << tableName << "' no existe.\n";
        return false;
    }

    std::string query = "DROP TABLE " + tableName + ";";
    return _db.insertar(query, {});
}

// Agregar columna
bool Schema::addColumn(const std::string& tableName, const std::string& columnName, const std::string& columnType) {
    if (!_db.existe_tabla(tableName)) {
        std::cerr << "Error: La tabla '" << tableName << "' no existe.\n";
        return false;
    }

    std::string query = "ALTER TABLE " + tableName + " ADD COLUMN " + columnName + " " + columnType + ";";
    return _db.insertar(query, {});
}

// Eliminar columna
bool Schema::dropColumn(const std::string& tableName, const std::string& columnName) {
    if (!_db.existe_tabla(tableName)) {
        std::cerr << "Error: La tabla '" << tableName << "' no existe.\n";
        return false;
    }

    // Nota: SQLite no soporta DROP COLUMN directamente.
    std::string query = "ALTER TABLE " + tableName + " DROP COLUMN " + columnName + ";";
    return _db.insertar(query, {});
}
