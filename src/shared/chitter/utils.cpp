//
// Created by zeke on 11/4/18.
//

#include "utils.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <pugixml.hpp>
#include <range/v3/all.hpp>

namespace chitter {

    DbConfig loadConfig(const std::string fileName) {
        using namespace pugi;

        xml_document doc;
        xml_parse_result result = doc.load_file(fileName.c_str());
        if (result.status != xml_parse_status::status_ok) {
            throw std::runtime_error("error loading config file: " + fileName);
        }

        xml_node root = doc.first_child(); // should be 'config'

        // remove whitespace from each value string if it matches what we're looking for
        auto isSpace = [] (unsigned char c) { return std::isspace(c); };
        auto toString = [=] (const xml_node& node) {
            using namespace ranges;
            return std::string(node.child_value()) | action::remove_if(isSpace);
        };

        // naive xml tree parse
        unsigned int count = 0;
        DbConfig dbConfig;
        using namespace std::string_literals;
        for (xml_node node : root.children()) {
            if (node.name() == "databaseType"s) {

                dbConfig.type = toString(node);
                count++;
            } else if (node.name() == "username"s) {

                dbConfig.username = toString(node);
                count++;
            } else if (node.name() == "password"s) {

                dbConfig.password = toString(node);
                count++;
            } else if (node.name() == "ip"s) {

                dbConfig.ip = toString(node);
                count++;
            } else if (node.name() == "port"s) {

                dbConfig.port = toString(node);
                count++;
            } else if (node.name() == "databaseName"s) {

                dbConfig.name = toString(node);
                count++;
            }
        }

        if (count < 6) {
            throw std::runtime_error("not all fields in xml file");
        }

        return dbConfig;
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

