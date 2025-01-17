#ifndef MODELO_H
#define MODELO_H

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <functional>
#include "../include/json.hpp"
#include "dbController.h"

class Modelo {
public:
    // Constructor
    explicit Modelo(const std::string& tableName = "default_table", const std::string& idModel = "id");
     
    // Destructor virtual para permitir polimorfismo
    virtual ~Modelo() = default;
    bool is_valid=false;
    std::map<std::string, std::string> attributes; // Almacenar atributos del modelo
    // Métodos CRUD
    virtual bool save();
    Modelo* findOne(int idt);

    template <typename T>
    std::shared_ptr<T> belongsAllTo(const std::string& related_model, const std::string& foreign_key = "", const std::string& local_key = "") {
        std::string fk = foreign_key.empty() ? related_model + "_id" : foreign_key;
        std::string lk = local_key.empty() ? _idModel : local_key;

        auto related_row = _db.where(related_model, {{fk, {"=", attributes[lk]}}});
        if (!related_row.empty()) {
            std::shared_ptr<T> related_instance = std::make_shared<T>();
            for (const auto& prop : related_instance->props()) {
                related_instance->setAttribute(prop, related_row[0].at(prop));
            }
            return related_instance;
        }
        return nullptr; // Devuelve nullptr si no se encuentra el elemento relacionado
    }
    template <typename T>
    std::vector<T*> whereAll(const std::map<std::string, std::pair<std::string, std::string>>& conditions) {
        auto rows = _db.where(_table, conditions); // Consultar la base de datos
        std::vector<T*> resultados;

        for (const auto& row : rows) {
            T* obj = new T(); // Crear una instancia del tipo derivado
            for (const auto& prop : props()) {
                obj->setAttribute(prop, row.at(prop)); // Asignar valores de la fila a los atributos
            }
            resultados.push_back(obj);
        }

        return resultados;
    }

    std::vector<Modelo*> where(const std::map<std::string, std::pair<std::string, std::string>>& conditions);
    std::vector<Modelo*> all();

    // Convertir desde y hacia JSON
    Modelo* from_json(const std::string& json_str);
    std::string toJSON();
    nlohmann::json toDIC();

    // Relaciones
    std::vector<Modelo*> hasMany(const std::string& related_model, const std::string& foreign_key = "", const std::string& local_key = "");
    Modelo* belongsTo(const std::string& related_model, const std::string& foreign_key = "", const std::string& local_key = "");
    Modelo* hasOne(const std::string& related_model, const std::string& foreign_key = "", const std::string& local_key = "");

    // Métodos para manejar atributos
    void setAttribute(const std::string& key, const std::string& value);
    std::string getAttribute(const std::string& key) const;

    // Obtener atributos visibles
    std::vector<std::string> props();

    // Métodos para registrar y crear instancias dinámicamente
    static void registerClass(const std::string& className, std::function<std::unique_ptr<Modelo>()> creator);
    static std::unique_ptr<Modelo> createInstance(const std::string& className);

protected:
   
    dbController _db;                              // Controlador de base de datos

private:
    
    std::string _table;  // Nombre de la tabla
    std::string _idModel; // Nombre de la columna ID

    // Mapa estático para el Factory Pattern
    static std::map<std::string, std::function<std::unique_ptr<Modelo>()>> factoryMap;
};

#endif // MODELO_H
