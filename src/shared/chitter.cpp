//
// Created by zeke on 8/22/18.
//

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <experimental/filesystem>
#include <algorithm>
#include "chitter.h"

namespace fs = std::experimental::filesystem::v1;

void chitter::load_config() {
    const std::string FILE_NAME = fs::current_path() / "config";
    std::ifstream infile (FILE_NAME, std::ifstream::in);
    std::string line;
    std::getline(infile, line);
    std::getline(infile, line);
    // second line is what holds actual info.
    std::replace(line.begin(), line.end(), ',', ' ');
    std::stringstream ss (line);
    ss >> db::type >> db::username >> db::password
       >> db::ip >> db::port >> db::name;
}

pqxx::connection chitter::initiate() {
    return pqxx::connection (
            "user=" + db::username +
            " host=" + db::ip +
            " password=" + db::password +
            " dbname=" + db::name
            );
}

bool chitter::checkUserExists(const std::string userID) {
    pqxx::connection connection = chitter::initiate();
    pqxx::work work(connection);
    pqxx::result result = work.exec("SELECT userid FROM Users");
    work.commit();
    bool userExists = false;
    for (const auto& row : result) {
        const pqxx::field resultUserID = row[0];
        if (resultUserID.c_str() == userID) {
            userExists = true;
            break;
        }
    }
    return userExists;
}

bool chitter::verifyPassword(const std::string userID, const std::string password) {
    pqxx::connection connection = chitter::initiate();
    pqxx::work work(connection);
    std::stringstream ss;
    ss << "SELECT password FROM Users WHERE userid = " << userID;
    pqxx::result result = work.exec(ss.str());
    work.commit();
    bool passwordMatches = false;
    for (const auto& row : result) {
        const pqxx::field resultPassword = row[0];
        if (resultPassword.c_str() == password) {
            passwordMatches = true;
            break;
        }
    }
    return passwordMatches;
}


int main(int argc, char **argv) {
    try {
        chitter::load_config();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
