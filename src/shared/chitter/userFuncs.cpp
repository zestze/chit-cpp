//
// Created by zeke on 11/4/18.
//

#include "userFuncs.h"
#include "utils.h"

namespace chitter {

    bool checkUserExists(const std::string userID, pqxx::connection& connection) {
        pqxx::work work(connection);
        std::stringstream ss;
        ss << "SELECT COUNT(1) "
              "FROM Users "
              "WHERE userID = " << work.quote(userID);
        const pqxx::result RESULT = work.exec(ss.str());
        work.commit();
        return RESULT[0][0].as<bool>();
    }

    bool verifyPassword(const std::string userID, const std::string password,
                        pqxx::connection& connection) {
        // @TODO: include proper password checking mechanism.
        // @TODO: can also simplify to not scan, since only row should be returned.
        pqxx::work work(connection);
        std::stringstream ss;
        ss << "SELECT password FROM Users WHERE userid = " << work.quote(userID);
        const pqxx::result RESULT = work.exec(ss.str());
        work.commit();
        const std::string PASSWORD_RESULT = RESULT[0][0].as<std::string>();
        bool PASSWORD_MATCHES = (PASSWORD_RESULT == password); //@TODO: modify this to do proper password
        return PASSWORD_MATCHES;
    }

    void updatePassword(const std::string userID, const std::string newPassword,
                        pqxx::connection& connection) {
        pqxx::work work(connection);
        std::stringstream ss;
        ss << "UPDATE Users "
              "SET password = " << work.quote(newPassword) << " "
              "WHERE userID = " << work.quote(userID);
        const pqxx::result RESULT = work.exec(ss.str());
        work.commit();
    }

    void insertUser(const User& user, pqxx::connection& connection) {
        pqxx::work work(connection);
        std::stringstream ss;
        ss << "INSERT INTO Users (userID, password, realName, whoami) "
              "VALUES ";
        passValues(ss, work, {user.get_nick(), user.get_pass(),
                              user.get_real(), user.get_whoami()});
        const pqxx::result RESULT = work.exec(ss.str());
        work.commit();
    }

    bool handleUser(const User& user, pqxx::connection& connection) {
        const bool USER_EXISTS = checkUserExists(user.get_nick(), connection);
        bool passwordIsCorrect = true;
        if (USER_EXISTS) {
            passwordIsCorrect = verifyPassword(user.get_nick(), user.get_pass(),connection);
        } else {
            insertUser(user, connection);
        }
        return passwordIsCorrect;
    }

    std::string getBio(const std::string userID, pqxx::connection& connection) {
        pqxx::work work(connection);
        std::stringstream ss;
        ss << "SELECT bio "
              "FROM Users "
              "WHERE userID = " << work.quote(userID);
        const pqxx::result RESULT = work.exec(ss.str());
        work.commit();
        return RESULT[0][0].as<std::string>();
    }

    void updateBio(const std::string userID, const std::string bio,
                   pqxx::connection& connection) {
        pqxx::work work(connection);
        std::stringstream ss;
        ss << "UPDATE Users "
              "SET bio = " << work.quote(bio) <<
           "WHERE userID = " << work.quote(userID);
        const pqxx::result RESULT = work.exec(ss.str());
        work.commit();
    }

    void insertLogin(const std::string userID, const tcp::endpoint& endpoint,
            pqxx::connection& connection) {
        //@TODO: figure out how to get longitude and latitude
        std::string datetime = getCurrentDatetime(connection);
        pqxx::work work(connection);
        const std::string IP = endpoint.address().to_string();
        const std::string PORT = std::to_string(endpoint.port());
        std::stringstream ss;
        ss << "INSERT INTO UserMetaData (loginIp, loginPort, loginTime, userID) "
              "VALUES ";
        passValues(ss, work, {IP, PORT, datetime, userID});
        const pqxx::result RESULT = work.exec(ss.str());
        work.commit();
    }

    bool checkAlreadyFriends(const std::string friend1ID, const std::string friend2ID,
                                      pqxx::connection &connection) {
        pqxx::work work (connection);
        std::stringstream ss;
        ss << "SELECT COUNT (1) "
              "FROM Friends "
              "WHERE (friend1ID = " << work.quote(friend1ID) << " AND friend2ID = " << work.quote(friend2ID) << ") ";
        const pqxx::result RESULT = work.exec(ss.str());
        work.commit();
        return RESULT[0][0].as<bool>();
    }

    void insertFriend(const std::string userID, const std::string friendUserID,
                               pqxx::connection &connection) {
        std::string dateTime = getCurrentDatetime(connection);
        pqxx::work work (connection);
        std::stringstream ss;
        ss << "INSERT INTO Friends (friend1ID, friend2ID, established) "
              "VALUES ";
        passValues(ss, work, {userID, friendUserID, dateTime});
        const pqxx::result RESULT = work.exec(ss.str());
        work.commit();
    }

    std::vector<std::string> fetchFriends(const std::string userID, pqxx::connection &connection) {
        pqxx::work work (connection);
        std::stringstream ss;
        ss << "SELECT DISTINCT friend1ID, friend2ID "
              "FROM Friends "
              "WHERE friend1ID = " << work.quote(userID) << " OR friend2ID = " << work.quote(userID);
        const pqxx::result RESULT = work.exec(ss.str());
        work.commit();

        // now, need to collect and prune results.
        std::vector<std::string> ids;
        auto isRedundant = [&ids](const std::string id){
            return std::find(ids.begin(), ids.end(), id) != ids.end();
        };

        for (const auto& ROW : RESULT) {
            for (const auto& R : ROW) {
                const std::string ID = R.as<std::string>();
                const bool IRRELEVANT = (ID == userID) || isRedundant(ID);
                if (!IRRELEVANT)
                    ids.push_back(ID);
            }
        }
        return ids;
    }

}
