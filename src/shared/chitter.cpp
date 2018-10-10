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
#include <tuple>
#include "chitter.h"

namespace fs = std::experimental::filesystem::v1;
using namespace std::literals::string_literals;

template <class T>
T& chitter::operator << (T& stream, const Status statusEnum) {
    switch (statusEnum) {
        case Status::Admin:  stream << "Admin";  break;
        case Status::User:   stream << "User";   break;
        case Status::Banned: stream << "Banned"; break;
        case Status::Nonexistent: stream << "Nonexistent"; break;
        default: break;
    }
    return stream;
}

std::string chitter::getStatusString(const Status statusEnum) {
    std::string statusString;
    switch (statusEnum) {
        case Status::Admin:  statusString = "Admin";  break;
        case Status::User:   statusString = "User";   break;
        case Status::Banned: statusString = "Banned"; break;
        case Status::Nonexistent: statusString = "Nonexistent"; break;
        default: break;
    }
    return statusString;
}

chitter::Status chitter::getStatusEnum(const std::string statusString) {
    Status statusEnum;
    if (statusString == "Admin") {
        statusEnum = Status::Admin;
    } else if (statusString == "User") {
        statusEnum = Status::User;
    } else if (statusString == "Banned") {
        statusEnum = Status::Banned;
    } else if (statusString == "Nonexistent") {
        statusEnum = Status::Nonexistent;
    } else {
        throw std::runtime_error("don't recognize status string");
    }
    return statusEnum;
}

/*
chitter::DbConfig chitter::loadConfig() {
    // going to modify loadConfig
    const std::string FILE_NAME = fs::current_path() / "config";
    std::ifstream infile (FILE_NAME, std::ifstream::in);
    std::string line;
    std::getline(infile, line);
    std::getline(infile, line);
    // second line is what holds actual info.
    std::replace(line.begin(), line.end(), ',', ' ');
    std::stringstream ss (line);
    DbConfig db;
    ss >> db.type >> db.username >> db.password
       >> db.ip >> db.port >> db.name;
    return db;
}
 */

chitter::DbConfig chitter::loadConfig(const std::string fileName) {
    std::ifstream infile (fileName, std::ifstream::in);
    std::string line;
    std::getline (infile, line);
    std::getline (infile, line);
    // second line is what holds actual info.
    std::replace (line.begin(), line.end(), ',', ' ');
    std::stringstream ss (line);
    DbConfig db;
    ss >> db.type >> db.username >> db.password
       >> db.ip >> db.port >> db.name;
    return db;
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

pqxx::connection chitter::initiate(const std::string fileName) {
    return initiate(loadConfig(fileName));
}

pqxx::connection chitter::initiate(const DbConfig db) {
    return pqxx::connection (
            "user=" + db.username +
            " host=" + db.ip +
            " password=" + db.password +
            " dbname=" + db.name
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
    return result[0][0].as<bool>();
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
    std::string passwordResult = result[0][0].as<std::string>();
    bool PASSWORD_MATCHES = (passwordResult == password); //@TODO: modify this to do proper password
    return PASSWORD_MATCHES;
}

bool chitter::verifyPassword(const std::string userID, const std::string password) {
    pqxx::connection connection = initiate();
    return verifyPassword(userID, password, connection);
}

void chitter::updatePassword(const std::string userID, const std::string newPassword,
                                pqxx::connection& connection) {
    pqxx::work work(connection);
    std::stringstream ss;
    ss << "UPDATE Users "
          "SET password = " << work.quote(newPassword) << " "
          "WHERE userID = " << work.quote(userID);
    pqxx::result result = work.exec(ss.str());
    work.commit();
}

void chitter::updatePassword(const std::string userID, const std::string newPassword) {
    pqxx::connection connection = initiate();
    return updatePassword(userID, newPassword, connection);
}

void chitter::insertUser(const User& user, pqxx::connection& connection) {
    pqxx::work work(connection);
    std::stringstream ss;
    ss << "INSERT INTO Users (userID, password, realName, whoami) "
          "VALUES ";
    passValues(ss, work, {user.get_nick(), user.get_pass(),
               user.get_real(), user.get_whoami()});
    // @TODO: change user.user to user.whoami
    pqxx::result result = work.exec(ss.str());
    work.commit();

}

void chitter::insertUser(const User &user) {
    pqxx::connection connection = initiate();
    return insertUser(user, connection);
}

bool chitter::handleUser(const User& user, pqxx::connection& connection) {
    const bool USER_EXISTS = checkUserExists(user.get_nick(), connection);
    bool passwordIsCorrect = true;
    if (USER_EXISTS) {
        passwordIsCorrect = verifyPassword(user.get_nick(), user.get_pass(),connection);
    } else {
        insertUser(user, connection);
    }
    return passwordIsCorrect;
}

bool chitter::handleUser(const User& user) {
    pqxx::connection connection = initiate();
    return handleUser(user, connection);
}

std::string chitter::getBio(const std::string userID, pqxx::connection& connection) {
    pqxx::work work(connection);
    std::stringstream ss;
    ss << "SELECT bio "
          "FROM Users "
          "WHERE userID = " << work.quote(userID);
    pqxx::result result = work.exec(ss.str());
    work.commit();
    return result[0][0].as<std::string>();
}

std::string chitter::getBio(const std::string userID) {
    pqxx::connection connection = initiate();
    return getBio(userID, connection);
}

void chitter::updateBio(const std::string userID, const std::string bio, pqxx::connection& connection) {
    pqxx::work work(connection);
    std::stringstream ss;
    ss << "UPDATE Users "
          "SET bio = " << work.quote(bio) <<
          "WHERE userID = " << work.quote(userID);
    pqxx::result result = work.exec(ss.str());
    work.commit();
}

void chitter::updateBio(const std::string userID, const std::string bio) {
    pqxx::connection connection = initiate();
    return updateBio(userID, bio, connection);
}

std::string chitter::getCurrentDatetime(pqxx::connection& connection) {
    pqxx::work work(connection);
    std::stringstream ss;
    ss << "SELECT CURRENT_TIMESTAMP";
    pqxx::result result = work.exec(ss.str());
    work.commit();
    return result[0][0].as<std::string>();
}

std::string chitter::getCurrentDatetime() {
    pqxx::connection connection = initiate();
    return getCurrentDatetime(connection);
}

void chitter::insertLogin(const std::string userID,
    const tcp::endpoint& endpoint, pqxx::connection& connection) {
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

void chitter::insertLogin(const std::string userID, const tcp::endpoint& endpoint) {
    pqxx::connection connection = initiate();
    return insertLogin(userID, endpoint, connection);
}

bool chitter::checkServerExists(const std::string serverID, pqxx::connection& connection) {
    pqxx::work work(connection);
    std::stringstream ss;
    ss << "SELECT COUNT(1) "
          "FROM Servers "
          "WHERE serverID = " << work.quote(serverID);
    pqxx::result result = work.exec(ss.str());
    work.commit();
    return result[0][0].as<bool>();
}

bool chitter::checkServerExists(const std::string serverID) {
    pqxx::connection connection = initiate();
    return checkServerExists(serverID, connection);
}

void chitter::insertServer(const std::string serverID, pqxx::connection& connection) {
    pqxx::work work(connection);
    std::stringstream ss;
    ss << "INSERT INTO Servers (serverName) "
          " VALUES (" << work.quote(serverID) << ")";
    pqxx::result result = work.exec(ss.str());
    work.commit();
}

void chitter::insertServer(const std::string serverID) {
    pqxx::connection connection = initiate();
    return insertServer(serverID, connection);
}

void chitter::insertServerMetadata(const std::string serverID, const tcp::endpoint &endpoint,
                                   pqxx::connection& connection) {
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
}

void chitter::insertServerMetadata(const std::string serverID, const tcp::endpoint &endpoint) {
    pqxx::connection connection = initiate();
    return insertServerMetadata(serverID, endpoint, connection);
}

bool chitter::checkChannelExists(const std::string channelName, pqxx::connection& connection) {
    pqxx::work work(connection);
    std::stringstream ss;
    ss << "SELECT COUNT(1) "
          "FROM Channels "
          "WHERE channelName = " << work.quote(channelName);
    pqxx::result result = work.exec(ss.str());
    work.commit();
    return result[0][0].as<bool>();
}

bool chitter::checkChannelExists(const std::string channelName) {
    pqxx::connection connection = initiate();
    return checkChannelExists(channelName, connection);
}

void chitter::insertChannel(const std::string channelName, const std::string channelTopic,
                            const std::string serverName, pqxx::connection& connection) {
    pqxx::work work(connection);
    std::stringstream ss;
    ss << "INSERT INTO Channels (channelName, channelTopic, serverName) "
          " VALUES ";
    passValues(ss, work, {channelName, channelTopic, serverName});
    pqxx::result result = work.exec(ss.str());
    work.commit();
}

void chitter::insertChannel(const std::string channelName, const std::string channelTopic, const std::string serverName) {
    pqxx::connection connection = initiate();
    return insertChannel(channelName, channelTopic, serverName, connection);
}

void chitter::insertMsg(const std::string channelName, const std::string userID, const std::string msg,
                        const std::string serverName, pqxx::connection& connection) {
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

void chitter::insertMsg(const std::string channelName, const std::string userID, const std::string msg,
                        const std::string serverName) {
    pqxx::connection connection = initiate();
    return insertMsg(channelName, userID, msg, serverName, connection);
}

void chitter::insertConnection(const std::string channelID, const User &user, const chitter::Status status,
                               const std::string serverName, pqxx::connection &connection) {
    pqxx::work work(connection);
    std::stringstream ss;
    ss << "INSERT INTO Connections (userName, serverName, connStatus, displayName)"
          " VALUES ";
    std::stringstream workAround;
    workAround << status;
    passValues(ss, work, {user.get_nick(), serverName, workAround.str(), user.get_nick()});
    pqxx::result result = work.exec(ss.str());
    work.commit();
}

void chitter::insertConnection(const std::string channelID, const User &user, const chitter::Status status,
                               const std::string serverName) {
    pqxx::connection connection = initiate();
    return insertConnection(channelID, user, status, serverName, connection);
}

//void chitter::insertConnection(const std::string channelID, const User &user, const chitter::Status status,
//                               const std::string serverName, const pqxx::connection &connection) {
//    pqxx::work work(const_cast<pqxx::connection&>(connection));
//}

std::tuple<chitter::Status, std::string> chitter::getServerRoles(const std::string userID, const std::string serverName) {
    pqxx::connection connection = initiate();
    return getServerRoles(userID, serverName, connection);
}

std::tuple<chitter::Status, std::string> chitter::getServerRoles(const std::string userID, const std::string serverName, pqxx::connection &connection) {
    pqxx::work work(connection);
    std::stringstream ss;
    ss << "SELECT permissions, displayName FROM ServerRoles"
          " WHERE userID = " << work.quote(userID) << " AND serverName = " <<
       work.quote(serverName);
    pqxx::result result = work.exec(ss.str());
    work.commit();
    return std::tuple<chitter::Status, std::string>{ getStatusEnum(result[0][0].as<std::string>()),
                  result[0][1].as<std::string>()};
}

void chitter::insertServerRoles(const std::string userID, const std::string serverName,
                                const chitter::Status statusEnum, StringOpt displayName) {
    pqxx::connection connection = initiate();
    insertServerRoles(userID, serverName, statusEnum, displayName, connection);
}

void chitter::insertServerRoles(const std::string userID, const std::string serverName,
                                const chitter::Status statusEnum, StringOpt displayName,
                                pqxx::connection &connection) {
    pqxx::work work(connection);
    if (!displayName) {
        *displayName = userID;
    }
    std::stringstream ss;
    ss << "INSERT INTO ServerRoles (userID, serverName, permissions, displayName)"
          " VALUES ";
    passValues(ss, work, {userID, serverName, getStatusString(statusEnum), *displayName});
    pqxx::result result = work.exec(ss.str());
    work.commit();
}

chitter::Status chitter::getChannelRoles(const std::string userID, const std::string channelName,
                                         const std::string serverName) {
    pqxx::connection connection = initiate();
    return getChannelRoles(userID, channelName, serverName, connection);
}

chitter::Status chitter::getChannelRoles(const std::string userID, const std::string channelName,
                                         const std::string serverName, pqxx::connection &connection) {
    pqxx::work work(connection);
    std::stringstream ss;
    ss << "SELECT (permissions) FROM ChannelRoles "
          "WHERE userID = " << work.quote(userID) << " AND channelName = " << work.quote(channelName)
       << " AND serverName = " << work.quote(serverName);
    pqxx::result result = work.exec(ss.str());
    work.commit();
    Status statusEnum;
    if (result.size() == 0)
        statusEnum = Status::Nonexistent;
    else
        statusEnum = getStatusEnum(result[0][0].as<std::string>());
    return statusEnum;
}

void chitter::insertChannelRoles(const std::string userID, const std::string channelName, const std::string serverName,
                                 const chitter::Status statusEnum) {
    pqxx::connection connection = initiate();
    return insertChannelRoles(userID, channelName, serverName, statusEnum, connection);
}

void chitter::insertChannelRoles(const std::string userID, const std::string channelName, const std::string serverName,
                                 const chitter::Status statusEnum, pqxx::connection &connection) {
    pqxx::work work(connection);
    std::stringstream ss;
    ss << "INSERT INTO ChannelRoles (userID, channelName, serverName, permissions) "
          "VALUES ";
    passValues(ss, work, {userID, channelName, serverName, getStatusString(statusEnum)});
    pqxx::result result = work.exec(ss.str());
    work.commit();
}

bool chitter::checkAlreadyFriends(const std::string friend1ID, const std::string friend2ID) {
    pqxx::connection connection = initiate();
    return checkAlreadyFriends(friend1ID, friend2ID, connection);
}

bool chitter::checkAlreadyFriends(const std::string friend1ID, const std::string friend2ID,
                                  pqxx::connection &connection) {
    pqxx::work work (connection);
    std::stringstream ss;
    ss << "SELECT COUNT (1) "
          "FROM Friends "
          "WHERE (friend1ID = " << work.quote(friend1ID) << " AND friend2ID = " << work.quote(friend2ID) << ") "
          "OR (friend1ID = " << work.quote(friend2ID) << " AND friend2ID = " << work.quote(friend1ID) << ")";
    pqxx::result result = work.exec(ss.str());
    work.commit();
    return result[0][0].as<bool>();
}

void chitter::insertFriend(const std::string userID, const std::string friendUserID) {
    pqxx::connection connection = initiate();
    return insertFriend(userID, friendUserID, connection);
}

void chitter::insertFriend(const std::string userID, const std::string friendUserID,
                                                pqxx::connection &connection) {
    pqxx::work work (connection);
    std::stringstream ss;
    ss << "INSERT INTO Friends (friend1ID, friend2ID, established) "
          "VALUES ";
    passValues(ss, work, {userID, friendUserID, getCurrentDatetime()});
    work.exec(ss.str());
    work.commit();
}

std::vector<std::string> chitter::fetchFriends(const std::string userID) {
    pqxx::connection connection = initiate();
    return fetchFriends(userID, connection);
}

std::vector<std::string> chitter::fetchFriends(const std::string userID, pqxx::connection &connection) {
    pqxx::work work (connection);
    std::stringstream ss;
    ss << "SELECT DISTINCT friend1ID, friend2ID "
          "FROM Friends "
          "WHERE friend1ID = " << work.quote(userID) << " OR friend2ID = " << work.quote(userID);
    pqxx::result result = work.exec(ss.str());
    work.commit();

    // now, need to collect and prune results.
    std::vector<std::string> ids;
    auto isRedundant = [&ids](const std::string id){
        return std::find(ids.begin(), ids.end(), id) != ids.end();
    };
    for (const auto& row : result) {
        for (const auto& r : row) {
            std::string id = r.as<std::string>();
            const bool IRRELEVANT = (id == userID) || isRedundant(id);
            if (!IRRELEVANT)
                ids.push_back(id);
        }
    }
    return ids;
}
