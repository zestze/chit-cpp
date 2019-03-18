//
// Created by zeke on 11/4/18.
//

#include "serverFuncs.h"
#include "utils.h"

namespace chitter {

    bool checkServerExists(const std::string serverName, pqxx::connection& connection) {
        pqxx::work work(connection);
        std::stringstream ss;
        ss << "SELECT COUNT(1) "
              "FROM Servers "
              "WHERE serverName = " << work.quote(serverName);
        const pqxx::result RESULT = work.exec(ss.str());
        work.commit();
        return RESULT[0][0].as<bool>();
    }


    void insertServer(const std::string serverName, pqxx::connection& connection) {
        pqxx::work work(connection);
        std::stringstream ss;
        ss << "INSERT INTO Servers (serverName) "
              " VALUES (" << work.quote(serverName) << ")";
        const pqxx::result RESULT = work.exec(ss.str());
        work.commit();
    }

    void insertServerMetadata(const std::string serverName, const tcp::endpoint &endpoint,
                                       pqxx::connection& connection) {
        //@TODO: find a way to get current longitude and latitude
        //@TODO: write a python server that gets longitude and latitude
        std::string dateTime = getCurrentDatetime(connection);
        pqxx::work work(connection);
        const std::string IP   = endpoint.address().to_string();
        const std::string PORT = std::to_string(endpoint.port());
        std::stringstream ss;
        ss << "INSERT INTO ServerMetadata (startupTime, privateIP, privatePort, serverName) "
              " VALUES ";
        passValues(ss, work, {dateTime, IP, PORT, serverName});
        const pqxx::result RESULT = work.exec(ss.str());
        work.commit();
    }

    void handleServer(const std::string serverName,
                               const tcp::endpoint& endpoint, pqxx::connection& connection) {
        const bool SERVER_EXISTS = checkServerExists(serverName, connection);
        if (!SERVER_EXISTS) {
            insertServer(serverName, connection);
        }
        insertServerMetadata(serverName, endpoint, connection);
    }

    std::tuple<Status, std::string> getServerRoles(const std::string userID, 
            const std::string serverName, pqxx::connection &connection) {
        pqxx::work work(connection);
        std::stringstream ss;
        ss << "SELECT permissions, displayName FROM ServerRoles"
              " WHERE userID = " << work.quote(userID) << " AND serverName = " <<
           work.quote(serverName);
        const pqxx::result RESULT = work.exec(ss.str());
        work.commit();

        return {getStatusEnum(RESULT[0][0].as<std::string>()),
            RESULT[0][1].as<std::string>()};
    }

    void insertServerRoles(const std::string userID, const std::string serverName,
                                    const Status statusEnum, 
                                    std::optional<std::string> displayName,
                                    pqxx::connection &connection) {
        pqxx::work work(connection);
        if (!displayName) {
            *displayName = userID;
        }
        std::stringstream ss;
        ss << "INSERT INTO ServerRoles (userID, serverName, permissions, displayName)"
              " VALUES ";
        passValues(ss, work, {userID, serverName, getStatusString(statusEnum), *displayName});
        const pqxx::result RESULT = work.exec(ss.str());
        work.commit();
    }
}
