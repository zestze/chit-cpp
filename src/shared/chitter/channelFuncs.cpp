//
// Created by zeke on 11/4/18.
//

#include "channelFuncs.h"
#include "utils.h"

namespace chitter {
    bool checkChannelExists(const std::string channelName, const std::string serverName,
                                     pqxx::connection& connection) {
        pqxx::work work(connection);
        std::stringstream ss;
        ss << "SELECT COUNT(1) "
              "FROM  Channels "
              "WHERE channelName = " << work.quote(channelName) <<
              "AND   serverName  = " << work.quote(serverName);
        const pqxx::result RESULT = work.exec(ss.str());
        work.commit();
        return RESULT[0][0].as<bool>();
    }

    void insertChannel(const std::string channelName, const std::string channelTopic,
                                const std::string serverName, pqxx::connection& connection) {
        pqxx::work work(connection);
        std::stringstream ss;
        ss << "INSERT INTO Channels (channelName, channelTopic, serverName) "
              " VALUES ";
        passValues(ss, work, {channelName, channelTopic, serverName});
        const pqxx::result RESULT = work.exec(ss.str());
        work.commit();
    }

    void updateChannelTopic(const std::string channelName, const std::string channelTopic,
                                     const std::string serverName, pqxx::connection& connection) {
        pqxx::work work(connection);
        std::stringstream ss;
        ss << "UPDATE Channels "
              "SET    ChannelTopic = " << work.quote(channelTopic) <<
              "WHERE  ChannelName = "  << work.quote(channelName)  <<
              "AND    ServerName = "   << work.quote(serverName);
        const pqxx::result RESULT = work.exec(ss.str());
        work.commit();
    }


    std::string getChannelTopic(const std::string channelName, const std::string serverName,
                                         pqxx::connection& connection) {
        pqxx::work work(connection);
        std::stringstream ss;
        ss << "SELECT ChannelTopic "
              "FROM   Channels "
              "WHERE  ChannelName = " << work.quote(channelName) <<
              "AND    ServerName  = " << work.quote(serverName);
        const pqxx::result RESULT = work.exec(ss.str());
        work.commit();
        return RESULT[0][0].as<std::string>();
    }

    void insertMsg(const std::string channelName, const std::string userID, const std::string msg,
                            const std::string serverName, pqxx::connection& connection) {
        std::string datetime = getCurrentDatetime(connection);
        pqxx::work work(connection);
        std::stringstream ss;
        //@TODO: note, pythong has msg = msg[msg.find(":") + 1:]
        ss << "INSERT INTO ChatLogs (userID, originTime, content, channelName, serverName) "
              "VALUES ";
        passValues(ss, work, {userID, datetime, msg, channelName, serverName});
        const pqxx::result RESULT = work.exec(ss.str());
        work.commit();
    }

    Status getChannelRoles(const std::string userID, const std::string channelName,
                                    const std::string serverName, pqxx::connection &connection) {
        pqxx::work work(connection);
        std::stringstream ss;
        ss << "SELECT (permissions) FROM ChannelRoles "
              "WHERE userID = " << work.quote(userID) << " AND channelName = " << work.quote(channelName)
           << " AND serverName = " << work.quote(serverName);
        const pqxx::result RESULT = work.exec(ss.str());
        work.commit();
        Status statusEnum;
        if (RESULT.size() == 0)
            statusEnum = Status::Nonexistent;
        else
            statusEnum = getStatusEnum(RESULT[0][0].as<std::string>());
        return statusEnum;
    }

    void insertChannelRoles(const std::string userID, const std::string channelName, const std::string serverName,
                                     const Status statusEnum, pqxx::connection &connection) {
        pqxx::work work(connection);
        std::stringstream ss;
        ss << "INSERT INTO ChannelRoles (userID, channelName, serverName, permissions) "
              "VALUES ";
        passValues(ss, work, {userID, channelName, serverName, getStatusString(statusEnum)});
        pqxx::result result = work.exec(ss.str());
        work.commit();
    }

}
