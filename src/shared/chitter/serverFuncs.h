//
// Created by zeke on 11/4/18.
//

#ifndef CHIT_CPP_SERVERFUNCS_H
#define CHIT_CPP_SERVERFUNCS_H

#include <pqxx/pqxx>
#include <asio.hpp>
#include <string>
#include <experimental/optional>
#include <status.h>

namespace chitter {
    using tcp = asio::ip::tcp;
    //@TODO: figure out a way around using an optional
    using StringOpt = std::experimental::optional<std::string>;

    bool checkServerExists(const std::string serverName, pqxx::connection& connection);

    void insertServer(const std::string serverName, pqxx::connection& connection);

    void insertServerMetadata(const std::string serverName, const tcp::endpoint& endpoint,
                              pqxx::connection& connection);

    void handleServer(const std::string serverName, const tcp::endpoint& endpoint,
                      pqxx::connection& connection);

    std::tuple<Status, std::string> getServerRoles(const std::string userId, const std::string serverName,
                                                   pqxx::connection& connection);

    void insertServerRoles(const std::string userID, const std::string serverName, const Status statusEnum,
                           StringOpt displayName, pqxx::connection& connection);

}

#endif //CHIT_CPP_SERVERFUNCS_H