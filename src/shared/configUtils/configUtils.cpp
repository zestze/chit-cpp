//
// Created by zeke on 3/20/19.
//

#include <range/v3/all.hpp>

#include "configUtils.h"

void configUtils::checkNode(const pugi::xml_node& node) {
    if (node == nullptr) {
        std::stringstream ss;
        ss << "error looking for <";
        ss << node.name() << "> node";
        throw std::runtime_error(ss.str());
    }
}

pugi::xml_node configUtils::loadSetup(const std::string& fileName) {
    using namespace pugi;

    xml_document doc;
    const xml_parse_result result = doc.load_file(fileName.c_str());
    if (result.status != xml_parse_status::status_ok) {
        throw std::runtime_error("error loading config file: " + fileName);
    }

    xml_node root = doc.child("config");
    checkNode(root);

    return root;
}

std::string configUtils::toString(const pugi::xml_node& node) {
    //@TODO: only remove leading or trailing whitespace
    auto isSpace = [] (unsigned char c) { return std::isspace(c); };
    using namespace ranges;
    return std::string(node.child_value()) | action::remove_if(isSpace);
}

configUtils::DbConfig configUtils::loadDbConfig(const std::string fileName) {
    using namespace pugi;

    xml_node root = loadSetup(fileName);

    root = root.child("databaseInfo");
    checkNode(root);

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
        throw std::runtime_error("not all fields in db xml file are present");
    }

    return dbConfig;
}

configUtils::ServerConfig configUtils::loadServerConfig(const std::string fileName) {
    using namespace pugi;

    xml_node root = loadSetup(fileName);

    root = root.child("serverInfo");
    checkNode(root);

    xml_node  node = root.child("defaultName");
    checkNode(node);
    ServerConfig serverConfig;
    serverConfig.defaultName = toString(node);

    node = root.child("defaultTopic");
    checkNode(node);
    serverConfig.defaultTopic = toString(node);

    return serverConfig;
}