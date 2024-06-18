#include <iostream>
#include <sqlite3.h>
#include <vector>
#include <string>

// Estrutura para armazenar os dados
struct FuelData {
    std::string datetime;
    double level;
};

// Função para buscar os dados do banco de dados
std::vector<FuelData> get_data() {
    sqlite3* db;
    sqlite3_stmt* stmt;
    std::vector<FuelData> data;

    // Abrir conexão com o banco de dados
    int rc = sqlite3_open("fuel_tank_data.db", &db);
    if (rc) {
        std::cerr << "Não foi possível abrir o banco de dados: " << sqlite3_errmsg(db) << std::endl;
        return data;
    }

    // Preparar a consulta SQL
    const char* sql = "SELECT datetime, level FROM fuel_data ORDER BY datetime DESC";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Erro ao preparar a consulta: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return data;
    }

    // Executar a consulta e buscar os resultados
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        FuelData row;
        row.datetime = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        row.level = sqlite3_column_double(stmt, 1);
        data.push_back(row);
    }

    // Limpar e fechar a conexão
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return data;
}

// How to run: g++ -o teste  bd.cpp -lsqlite3 && ./teste

int main() {
    std::vector<FuelData> data = get_data();

    // Imprimir os dados
    for (const auto& row : data) {
        std::cout << "Datetime: " << row.datetime << ", Level: " << row.level << std::endl;
    }

    return 0;
}
