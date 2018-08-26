//
// Created by zeke on 8/22/18.
//

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <experimental/filesystem>
#include <algorithm>
#include <vector>
#include "chitter.h"
//@TODO: finish going though other functions and adding a second overload to them
//@TODO: akin to checkUserExists and verifyPassword

namespace fs = std::experimental::filesystem::v1;
using namespace std::literals::string_literals;

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

void printResult(const pqxx::result result) {
    for (int i = 0; i < result.size(); i++) {
        for (int j = 0; j < result[i].size(); j++) {
            std::cout << "result[" << i << "][" << j << "] == " << result[i][j] << std::endl;
        }
    }
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

//@TODO: replace vector impl with a variadic template
template<typename T, typename... Targs>
void gatherArgs(T t, Targs... targs) {

}

pqxx::connection chitter::initiate() {
    return pqxx::connection (
            "user=" + db::username +
            " host=" + db::ip +
            " password=" + db::password +
            " dbname=" + db::name
            );
}

bool chitter::checkUserExists(const std::string userID, pqxx::connection& connection) {
    pqxx::work work(connection);
    std::stringstream ss;
    ss << "SELECT COUNT(1) "
          "FROM Users "
          "WHERE userID = " << work.quote(userID);
    pqxx::result result = work.exec(ss.str());
    work.commit();
    return result[0][0].as<bool>() == true;
}

bool chitter::checkUserExists(const std::string userID) {
    pqxx::connection connection = initiate();
    return checkUserExists(userID, connection);
}

bool chitter::verifyPassword(const std::string userID, const std::string password,
                                pqxx::connection& connection) {
    // @TODO: include proper password cchecking mechanism.
    // @TODO: can also simplify to not scan, since only row should be returned.
    pqxx::work work(connection);
    std::stringstream ss;
    ss << "SELECT password FROM Users WHERE userid = " << work.quote(userID);
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

bool chitter::verifyPassword(const std::string userID, const std::string password) {
    pqxx::connection connection = initiate();
    return verifyPassword(userID, password, connection);
}

void chitter::updatePassword(const std::string userID, const std::string newPassword,
                                pqxx::connection connection) {
    pqxx::work work(connection);
    std::stringstream ss;
    ss << "UPDATE Users "
          "SET password = " << work.quote(newPassword) << " "
          "WHERE userID = " << work.quote(userID);
    pqxx::result result = work.exec(ss.str());
    work.commit();
}

void chitter::insertUser(const User& user, pqxx::connection connection) {
    pqxx::work work(connection);
    std::stringstream ss;

    ss << "INSERT INTO Users (userID, password, realName, whoami) "
          "VALUES ";
    passValues(ss, work, {user.get_nick(), user.get_pass(),
               user.get_real(), user.get_user()});
    // @TODO: change user.user to user.whoami
    pqxx::result result = work.exec(ss.str());
    work.commit();

}

std::string chitter::getBio(const std::string userID, pqxx::connection connection) {
    pqxx::work work(connection);
    std::stringstream ss;
    ss << "SELECT bio "
          "FROM Users "
          "WHERE userID = " << work.quote(userID);
    pqxx::result result = work.exec(ss.str());
    work.commit();
    return result[0][0].as<std::string>();
}

void chitter::updateBio(const std::string userID, const std::string bio, pqxx::connection connection) {
    pqxx::work work(connection);
    std::stringstream ss;
    ss << "UPDATE Users "
          "SET bio = " << work.quote(bio) <<
          "WHERE userID = " << work.quote(userID);
    pqxx::result result = work.exec(ss.str());
    work.commit();
}

std::string chitter::getCurrentDatetime(pqxx::connection connection) {
    pqxx::work work(connection);
    std::stringstream ss;
    ss << "SELECT CURRENT_TIMESTAMP";
    pqxx::result result = work.exec(ss.str());
    work.commit();
    return result[0][0].as<std::string>();
}

void chitter::insertLogin(const std::string userID,
    const tcp::endpoint& endpoint, pqxx::connection connection) {
    //@TODO: figure out how to get longitude and latitude
    pqxx::work work(connection);
    std::stringstream ss;
    std::string ip = endpoint.address().to_string();
    auto port = std::to_string(endpoint.port());
    std::string datetime = getCurrentDatetime();
    ss << "INSERT INTO UserMetaData (loginIp, loginPort, loginTime, userID) "
          "VALUES ";
    passValues(ss, work, {ip, port, datetime, userID});
    pqxx::result result = work.exec(ss.str());
    work.commit();
}

bool chitter::checkServerExists(const std::string serverID, pqxx::connection connection) {
    pqxx::work work(connection);
    std::stringstream ss;
    ss << "SELECT COUNT(1) "
          "FROM Servers "
          "WHERE serverID = " << work.quote(serverID);
    pqxx::result result = work.exec(ss.str());
    work.commit();
    return result[0][0].as<bool>();
}

void chitter::insertServer(const std::string serverID, pqxx::connection connection) {
    pqxx::work work(connection);
    std::stringstream ss;
    ss << "INSERT INTO Servers (serverName) "
          " VALUES (" << work.quote(serverID) << ")";
    pqxx::result result = work.exec(ss.str());
    work.commit();
}


void chitter::insertServerMetadata(const std::string serverID, const tcp::endpoint &endpoint,
                                   pqxx::connection connection) {
    //@TODO: find a way to get current longitude and latitude
    //@TODO: write a python server that gets longitude and latitude
    pqxx::work work(connection);
    std::string dateTime = getCurrentDatetime();
    std::string ip = endpoint.address().to_string();
    std::string port = std::to_string(endpoint.port());
    std::stringstream ss;
    ss << "INSERT INTO ServerMetadata (startupTime, privateIP, privatePort, serverName "
          " VALUES ";
    passValues(ss, work, {dateTime, ip, port, serverID});
    pqxx::result result = work.exec(ss.str());
    work.commit();
    //printResult(result);
}

bool chitter::checkChannelExists(const std::string channelName, pqxx::connection connection) {
    pqxx::work work(connection);
    std::stringstream ss;
    ss << "SELECT COUNT(1) "
          "FROM Channels "
          "WHERE channelName = " << work.quote(channelName);
    pqxx::result result = work.exec(ss.str());
    work.commit();
    return result[0][0].as<bool>();
}

void chitter::insertChannel(const std::string channelName, const std::string channelTopic,
                            const std::string serverName, pqxx::connection connection) {
    pqxx::work work(connection);
    std::stringstream ss;
    ss << "INSERT INTO Channels (channelName, channelTopic, serverName) "
          " VALUES ";
    passValues(ss, work, {channelName, channelTopic, serverName});
    pqxx::result result = work.exec(ss.str());
    work.commit();
}

void chitter::insertMsg(const std::string channelName, const std::string userID, const std::string msg,
                        const std::string serverName, pqxx::connection connection) {
    pqxx::work work(connection);
    std::stringstream ss;
    std::string datetime = getCurrentDatetime();
    //@TODO: note, pythong has msg = msg[msg.find(":") + 1:]
    ss << "INSERT INTO ChatLogs (userID, originTime, content, channelName, serverName) "
          "VALUES ";
    passValues(ss, work, {userID, datetime, msg, channelName, serverName});
    pqxx::result result = work.exec(ss.str());
    work.commit();
}

// @TODO: left off here, need to stat at insert_connection from chitter.

int main(int argc, char **argv) {
    try {
        //@TODO: move main stuff to a testing file
        chitter::load_config();
        std::cout << chitter::checkUserExists("zest") << std::endl;
        std::cout << chitter::checkUserExists("zest12342452345") << std::endl;
        std::cout << chitter::verifyPassword("zest", "") << std::endl;
        std::cout << chitter::verifyPassword("zest", "asdf") << std::endl;
        std::cout << chitter::verifyPassword("testPass", "testPass") << std::endl;
        std::cout << chitter::getBio("blah") << std::endl;
        tcp::endpoint end (asio::ip::address_v4::from_string("127.0.0.1"),
                        3372);
        //chitter::insertLogin("testPass", end);
        pqxx::connection connection = chitter::initiate();
        pqxx::work work (connection);
        std::stringstream ss;
        auto ip = end.address().to_string();
        auto port = std::to_string(end.port());
        passValues(ss, work, {ip, port});
        std::cout << ss.str() << std::endl;


    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
