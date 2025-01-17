#include "Modelo.h"
#include <iostream>

// Constructor
/*Modelo::Modelo(const std::string& tableName)
    : _db(), _table(tableName), _idModel("id") {  // Orden corregido de inicialización
    // Inicializar los atributos desde la base de datos
    auto fields = _db.getFields(_table);
    for (const auto& field : fields) {
        attributes[field] = ""; // Inicializar cada campo con una cadena vacía
    }
}*/

Modelo::Modelo(const std::string& tableName, const std::string& idModel)
    : _db(), _table(tableName), _idModel(idModel) {  // Orden corregido de inicialización
    // Inicializar los atributos desde la base de datos
    auto fields = _db.getFields(_table);
    for (const auto& field : fields) {
        attributes[field] = ""; // Inicializar cada campo con una cadena vacía
    }
}// Destructor
// Inicializar el mapa estático
std::map<std::string, std::function<std::unique_ptr<Modelo>()>> Modelo::factoryMap;

// Registrar clases en el Factory
void Modelo::registerClass(const std::string& className, std::function<std::unique_ptr<Modelo>()> creator) {
    factoryMap[className] = creator;
}

// Crear una instancia basada en el nombre de la clase
std::unique_ptr<Modelo> Modelo::createInstance(const std::string& className) {
    auto it = factoryMap.find(className);
    if (it != factoryMap.end()) {
        return (it->second)(); // Llama a la función creadora
    }
    throw std::runtime_error("Clase no registrada: " + className);
}


// Relaciones
std::vector<Modelo*> Modelo::hasMany(const std::string& related_model, const std::string& foreign_key, const std::string& local_key) {
    std::string fk = foreign_key.empty() ? _idModel : foreign_key;
    std::string lk = local_key.empty() ? _idModel : local_key;

    auto related_models = _db.where(related_model, {{fk, {"=", attributes[lk]}}});
    std::vector<Modelo*> results;

    for (const auto& row : related_models) {
        Modelo* obj = new Modelo(related_model);
        for (const auto& prop : props()) {
            obj->attributes[prop] = row.at(prop);
        }
        results.push_back(obj);
    }
    return results;
}

Modelo* Modelo::belongsTo(const std::string& related_model, const std::string& foreign_key, const std::string& local_key) {
    std::string fk = foreign_key.empty() ? related_model + "_id" : foreign_key;
    std::string lk = local_key.empty() ? _idModel : local_key;

    auto related_row = _db.where(related_model, {{_idModel, {"=", attributes[fk]}}});
    if (!related_row.empty()) {
        Modelo* related_instance = new Modelo(related_model);
        for (const auto& prop : props()) {
            related_instance->attributes[prop] = related_row[0].at(prop);
        }
        return related_instance;
    }
    return nullptr;
}
// Método para guardar (insertar o actualizar) un registro
bool Modelo::save() {
    // Verificar si el registro ya existe basado en la presencia del ID
    if (attributes[_idModel].empty()) {
        // Si no existe, insertar el nuevo registro
        return _db.insertar(_table, attributes);
    } else {
        // Si ya existe, actualizar el registro
        return _db.updateWhere(_table, attributes, {{_idModel, {"=", attributes[_idModel]}}});
    }
}
Modelo* Modelo::hasOne(const std::string& related_model, const std::string& foreign_key, const std::string& local_key) {
    std::string fk = foreign_key.empty() ? _idModel : foreign_key;
    std::string lk = local_key.empty() ? _idModel : local_key;

    auto related_row = _db.where(related_model, {{fk, {"=", attributes[lk]}}});
    if (!related_row.empty()) {
        Modelo* related_instance = new Modelo(related_model);
        for (const auto& prop : props()) {
            related_instance->attributes[prop] = related_row[0].at(prop);
        }
        return related_instance;
    }
    return nullptr;
}

// Método para buscar un registro por ID
Modelo* Modelo::findOne(int idt) {
    auto rows = _db.where(_table, {{_idModel, {"=", std::to_string(idt)}}});
    if (!rows.empty()) {
        for (const auto& prop : props()) {
            attributes[prop] = rows[0].at(prop); // Asignar valores a los atributos
        }
    }
    return this;
}

// Método where para obtener múltiples registros
std::vector<Modelo*> Modelo::where(const std::map<std::string, std::pair<std::string, std::string>>& conditions) {
    auto rows = _db.where(_table, conditions);
    std::vector<Modelo*> resultados;

    for (const auto& row : rows) {
        Modelo* obj = new Modelo(_table); // Crear una nueva instancia
        for (const auto& prop : props()) {
            obj->attributes[prop] = row.at(prop); // Asignar valores de la fila a los atributos
        }
        resultados.push_back(obj);
    }

    return resultados;
}
// The all() method
std::vector<Modelo*> Modelo::all() {
    std::vector<Modelo*> resultados;
    try {
        // Obtener todas las filas de la tabla
        std::unique_ptr<sql::ResultSet> rows(_db.selectall(_table));

        // Obtener el metadato una vez
        sql::ResultSetMetaData* metaData = rows->getMetaData();
        int columnCount = metaData->getColumnCount();

        // Recorrer cada fila y crear una instancia de Modelo
        while (rows->next()) {
            Modelo* obj = new Modelo(_table, _idModel);

            // Poblar los atributos de la instancia Modelo
            for (int i = 1; i <= columnCount; ++i) {
                std::string columnName = metaData->getColumnName(i);
                std::string columnValue = rows->getString(i);
                obj->attributes[columnName] = columnValue;
            }

            // Agregar la instancia de Modelo al vector de resultados
            resultados.push_back(obj);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error en all(): " << e.what() << std::endl;
    }
    return resultados;
}

// Método para convertir el modelo a un objeto JSON
nlohmann::json Modelo::toDIC() {
    nlohmann::json diccionario;
    for (const auto& prop : props()) {
        diccionario[prop] = attributes[prop]; // Convertir atributos a JSON
    }
    return diccionario;
}

// Método para convertir a JSON
std::string Modelo::toJSON() {
    nlohmann::json diccionario = toDIC();
    return diccionario.dump(); // Convertir a cadena JSON
}

// Método para convertir desde JSON
Modelo* Modelo::from_json(const std::string& json_str) {
    auto json_dict = nlohmann::json::parse(json_str);
    for (const auto& item : json_dict.items()) {  // Sin usar desestructuración
        attributes[item.key()] = item.value().get<std::string>(); // Asignar valores del JSON a los atributos
    }
    return this;
}

// Método props para obtener los atributos visibles
std::vector<std::string> Modelo::props() {
    std::vector<std::string> visibleProps;
    for (const auto& pair : attributes) {
        if (pair.first[0] != '_') { // Ignorar atributos privados
            visibleProps.push_back(pair.first);
        }
    }
    return visibleProps;
}
// Métodos para manejar atributos
void Modelo::setAttribute(const std::string& key, const std::string& value) {
    is_valid=true;
    attributes[key] = value;
}


std::string Modelo::getAttribute(const std::string& key) const {
    auto it = attributes.find(key);
    if (it != attributes.end()) {
        return it->second;
    }
    return "";
}

