#include "dbController.h"
#include "../GlobalConfig.h"
#include <memory>

 
// Constructor
dbController::dbController() {
    // Obtener la instancia de GlobalConfig
    GlobalConfig& config = GlobalConfig::getInstance();

    // Obtener la configuración de MySQL
    MySQLConfig mysqlConfig = config.getMySQLConfig();
    try {
        // Inicializar el driver de MySQL
        driver = sql::mysql::get_mysql_driver_instance();

        // Conectar a la base de datos usando la configuración obtenida del JSON
        _conn = std::unique_ptr<sql::Connection>(
            driver->connect(mysqlConfig.host, mysqlConfig.user, mysqlConfig.password)
        );

        // Seleccionar la base de datos
        _conn->setSchema(mysqlConfig.database);

    } catch (sql::SQLException &e) {
        std::cerr << "PETS> Error al conectar a la base de datos: " << e.what() << std::endl;
    }
}

// Destructor
dbController::~dbController() {
    // Implementar la limpieza o cierre de la conexión aquí
    if (_conn) {
        _conn->close();
    }
}
std::unique_ptr<sql::ResultSet> dbController::selectcomplete(const std::string& tabla, 
    const std::vector<std::map<std::string, std::string>>& condiciones, const std::string& joins) {
    std::unique_ptr<sql::PreparedStatement> stmt(_conn->prepareStatement("..."));

    // Ejecutar consulta
    sql::ResultSet* result = stmt->executeQuery();
    return std::unique_ptr<sql::ResultSet>(result);  // Envolver el puntero crudo en std::unique_ptr
}

std::unique_ptr<sql::ResultSet> dbController::select(const std::string& tabla, const std::string& campo, const std::string& simbolo, const std::string& valor) {
    std::unique_ptr<sql::PreparedStatement> stmt(_conn->prepareStatement("..."));

    // Ejecutar consulta
    sql::ResultSet* result = stmt->executeQuery();
    return std::unique_ptr<sql::ResultSet>(result);  // Envolver el puntero crudo en std::unique_ptr
}

std::unique_ptr<sql::ResultSet> dbController::selectall(const std::string& tabla) {
    std::unique_ptr<sql::PreparedStatement> stmt(_conn->prepareStatement("SELECT * FROM " + tabla));

    // Ejecutar consulta
    sql::ResultSet* result = stmt->executeQuery();
    return std::unique_ptr<sql::ResultSet>(result);  // Envolver el puntero crudo en std::unique_ptr
}

std::unique_ptr<sql::ResultSet> dbController::getColType(const std::string& tabla, const std::string& col) {
    std::unique_ptr<sql::PreparedStatement> stmt(_conn->prepareStatement("..."));

    // Ejecutar consulta
    sql::ResultSet* result = stmt->executeQuery();
    return std::unique_ptr<sql::ResultSet>(result);  // Envolver el puntero crudo en std::unique_ptr
}

std::unique_ptr<sql::ResultSet> dbController::getCols(const std::string& tabla) {
    std::unique_ptr<sql::PreparedStatement> stmt(_conn->prepareStatement("..."));

    // Ejecutar consulta
    sql::ResultSet* result = stmt->executeQuery();
    return std::unique_ptr<sql::ResultSet>(result);  // Envolver el puntero crudo en std::unique_ptr
}

std::unique_ptr<sql::ResultSet> dbController::getColPrimaryKey(const std::string& tabla) {
    std::unique_ptr<sql::PreparedStatement> stmt(_conn->prepareStatement("..."));

    // Ejecutar consulta
    sql::ResultSet* result = stmt->executeQuery();
    return std::unique_ptr<sql::ResultSet>(result);  // Envolver el puntero crudo en std::unique_ptr
}
// Método where agregado
std::vector<std::map<std::string, std::string>> dbController::where(
    const std::string& tabla, 
    const std::map<std::string, std::pair<std::string, std::string>>& filters) {

    // Crear la consulta SQL dinámica
    std::string query = "SELECT * FROM " + tabla + " WHERE ";
    bool first = true;

    for (const auto& filter : filters) {
        if (!first) {
            query += " AND ";
        }
        query += filter.first + " " + filter.second.first + " ?";
        first = false;
    }

    std::unique_ptr<sql::PreparedStatement> stmt(_conn->prepareStatement(query));

    // Asignar los valores a los parámetros de la consulta preparada
    int index = 1;
    for (const auto& filter : filters) {
        stmt->setString(index, filter.second.second);
        ++index;
    }

    // Ejecutar la consulta
    std::unique_ptr<sql::ResultSet> result(stmt->executeQuery());

    // Almacenar los resultados en un vector de mapas
    std::vector<std::map<std::string, std::string>> result_rows;
    sql::ResultSetMetaData* meta = result->getMetaData();
    int columnCount = meta->getColumnCount();

    while (result->next()) {
        std::map<std::string, std::string> row;
        for (int i = 1; i <= columnCount; ++i) {
            std::string columnName = meta->getColumnName(i);
            std::string columnValue = result->getString(i);
            row[columnName] = columnValue;
        }
        result_rows.push_back(row);
    }

    return result_rows;
}
std::vector<std::string> dbController::getFields(const std::string& table) {
    std::vector<std::string> fields;
    try {
        // Preparar consulta para obtener los metadatos de las columnas
        std::string query = "SELECT * FROM `" + table + "` LIMIT 1;";
        std::unique_ptr<sql::PreparedStatement> stmt(_conn->prepareStatement(query));
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());

        sql::ResultSetMetaData* metaData = res->getMetaData();
        int columnCount = metaData->getColumnCount();

        for (int i = 1; i <= columnCount; ++i) {
            fields.push_back(metaData->getColumnName(i));
        }
    } catch (const sql::SQLException& e) {
       GlobalTools::lomPrint("Error obteniendo los campos de la tabla6");
      
    }
    return fields;
}

bool dbController::insertar(const std::string& tabla, const std::map<std::string, std::string>& campos) {
    try {
        std::string query = "INSERT INTO " + tabla + " (";
        std::string values = "VALUES (";
        bool first = true;

        // Solo incluir campos que no estén vacíos en la consulta
        for (const auto& campo : campos) {
            if (!campo.second.empty()) {  // Ignorar campos con valores vacíos
                if (!first) {
                    query += ", ";
                    values += ", ";
                }
                query += campo.first;
                values += "?";
                first = false;
            }
        }

        query += ") " + values + ")";

        // Preparar la consulta SQL
        std::unique_ptr<sql::PreparedStatement> stmt(_conn->prepareStatement(query));
        int index = 1;

        // Asignar solo los valores no vacíos a los parámetros de la consulta preparada
        for (const auto& campo : campos) {
            if (!campo.second.empty()) {  // Solo asignar valores no vacíos
                stmt->setString(index, campo.second);
                ++index;
            }
        }

        stmt->execute();
        return true;  // Operación exitosa
    } catch (const sql::SQLException &e) {
        std::cerr << "PETS> Error al insertar: " << e.what() << std::endl;
        return false;  // Error durante la operación
    }
}

bool dbController::updateWhere(const std::string& tabla, const std::map<std::string, std::string>& campos,
                               const std::map<std::string, std::pair<std::string, std::string>>& condicion) {
    try {
        std::string query = "UPDATE " + tabla + " SET ";
        bool first = true;

        // Solo incluir campos con valores no vacíos
        for (const auto& campo : campos) {
            if (!campo.second.empty()) {  // Ignorar campos con valores vacíos
                if (!first) {
                    query += ", ";
                }
                query += campo.first + " = ?";
                first = false;
            }
        }

        // Si no hay campos para actualizar, retornar falso
        if (first) {
            std::cerr << "No hay campos válidos para actualizar." << std::endl;
            return false;
        }

        query += " WHERE ";
        first = true;

        for (const auto& cond : condicion) {
            if (!first) {
                query += " AND ";
            }
            query += cond.first + " " + cond.second.first + " ?";
            first = false;
        }

        std::unique_ptr<sql::PreparedStatement> stmt(_conn->prepareStatement(query));
        int index = 1;

        // Asignar solo los valores no vacíos a los parámetros de la consulta preparada
        for (const auto& campo : campos) {
            if (!campo.second.empty()) {  // Solo asignar valores no vacíos
                stmt->setString(index, campo.second);
                ++index;
            }
        }

        for (const auto& cond : condicion) {
            stmt->setString(index, cond.second.second);
            ++index;
        }

        stmt->execute();
        return true;  // Operación exitosa
    } catch (const sql::SQLException &e) {
        std::cerr << "PETS> Error al actualizar: " << e.what() << std::endl;
        return false;  // Error durante la operación
    }
}