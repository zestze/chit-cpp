//
// Created by zeke on 11/4/18.
//

#include "utils.h"
#include <fstream>
#include <iostream>

namespace chitter {

    DbConfig loadConfig(const std::string fileName) {
        std::ifstream infile (fileName, std::ifstream::in);
        std::string line;
        //@TODO: properly map first line to second
        std::getline (infile, line);
        std::getline (infile, line);
        // second line is what holds actual info
        std::replace(line.begin(), line.end(), ',', ' ');
        std::stringstream ss (line);
        DbConfig db;
        ss >> db.type >> db.username >> db.password
           >> db.ip   >> db.port     >> db.name;
        return db;
    }

    pqxx::connection initiate(const std::string fileName) {
        return initiate(loadConfig(fileName));
    }

    pqxx::connection initiate(const DbConfig dbConfig) {
        return pqxx::connection (
                "user=" + dbConfig.username  +
                " host=" + dbConfig.ip +
                " password=" + dbConfig.password +
                " dbname=" + dbConfig.name
                );
    }

    std::string getCurrentDatetime(pqxx::connection& connection) {
        pqxx::work work (connection);
        std::stringstream ss;
        ss << "SELECT CURRENT_TIMESTAMP";
        const pqxx::result RESULT = work.exec(ss.str());
        work.commit();
        return RESULT[0][0].as<std::string>();

    }

    void passValues(std::stringstream& ss, pqxx::work& work,
            std::vector<std::string> values) {
        ss << "(";
        for (int i = 0; i < values.size(); i++) {
            ss << work.quote(values[i]);
            if (i < values.size() - 1)
                ss << ", ";
        }
        ss << ")";
    }

    void printResult(const pqxx::result result) {
        for (int i = 0; i < result.size(); i++) {
            for (int j = 0; j < result[i].size(); j++) {
                std::cout << "result[" << i << "][" << j << "] == " << result[i][j] << std::endl;
            }
        }
    }
}

