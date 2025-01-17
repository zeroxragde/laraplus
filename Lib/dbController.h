#ifndef DBCONTROLLER_H
#define DBCONTROLLER_H

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/metadata.h>
#include <cppconn/resultset_metadata.h>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include "GlobalTools.h"
class dbController {
private:
    std::shared_ptr<sql::Connection> _conn;
    sql::mysql::MySQL_Driver *driver;
    std::vector<std::map<std::string, std::string>> _resultados;

public:
    
    // Constructor y Destructor
    dbController();
    ~dbController(); 
    // Métodos de control de conexión
    void connect(const std::string& host, const std::string& user, const std::string& password, const std::string& db);
    void close();

    // Métodos para operaciones CRUD
    bool insertar(const std::string& tabla, const std::map<std::string, std::string>& campos); // Cambia a:
    bool updateWhere(const std::string& tabla, const std::map<std::string, std::string>& campos,
                     const std::map<std::string, std::pair<std::string, std::string>>& condicion);
    std::vector<std::map<std::string, std::string>> where(const std::string& table,
                                                          const std::map<std::string, std::pair<std::string, std::string>>& filters);
    
    // Verificación de tabla
    bool existe_tabla(const std::string& nombre_tabla);

    // Métodos para obtener los resultados
    std::map<std::string, std::string> first();
    std::map<std::string, std::string> last();
    std::vector<std::map<std::string, std::string>> get();
    std::vector<std::string> getFields(const std::string& table);
    std::unique_ptr<sql::ResultSet> selectcomplete(const std::string& tabla, const std::vector<std::map<std::string, std::string>>& condiciones, const std::string& joins = "");
    std::unique_ptr<sql::ResultSet> select(const std::string& tabla, const std::string& simbolo, const std::string& campo, const std::string& valor);
    std::unique_ptr<sql::ResultSet> selectall(const std::string& tabla);
    std::unique_ptr<sql::ResultSet> getColType(const std::string& tabla, const std::string& col);
    std::unique_ptr<sql::ResultSet> getCols(const std::string& tabla);
    std::unique_ptr<sql::ResultSet> getColPrimaryKey(const std::string& tabla);
};

#endif // DBCONTROLLER_H
