//
// Created by zeke on 11/4/18.
//

#include "utils.h"
#include <fstream>
#include <iostream>
#include <algorithm>

namespace chitter {

    pqxx::connection initiate(const std::string fileName) {
        return initiate(configUtils::loadDbConfig(fileName));
    }

    pqxx::connection initiate(const configUtils::DbConfig dbConfig) {
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
            if (i < values.size() - 1) {
                ss << ", ";
            }
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

