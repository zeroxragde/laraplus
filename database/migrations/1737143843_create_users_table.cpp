#include "../Libs/Schema.h"

class create_users_table : public Schema {
public:
    void up() override {
        createTable("users", {
            {"id", "INT AUTO_INCREMENT PRIMARY KEY"},
            {"created_at", "DATETIME"},
            {"updated_at", "DATETIME"}
        });
    }

    void down() override {
        dropTable("users");
    }
};
