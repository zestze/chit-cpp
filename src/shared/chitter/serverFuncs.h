//
// Created by zeke on 11/4/18.
//

#ifndef CHIT_CPP_SERVERFUNCS_H
#define CHIT_CPP_SERVERFUNCS_H

#include <pqxx/pqxx>
#include <asio.hpp>
#include <string>
#include <status.h>


// protect against compilers that only have experimental support of optional
#ifdef __cpp_lib_optional
#include <optional>
template<class T>
using optional = std::optional<T>;
#elif  __cpp_lib_experimental_optional
template<class T>
using optional = std::experimental::optional<T>;
#include <experimental/optional>
#endif

namespace chitter {
    using tcp = asio::ip::tcp;

    bool checkServerExists(const std::string serverName, pqxx::connection& connection);

    void insertServer(const std::string serverName, pqxx::connection& connection);

    void insertServerMetadata(const std::string serverName, const tcp::endpoint& endpoint,
                              pqxx::connection& connection);

    void handleServer(const std::string serverName, const tcp::endpoint& endpoint,
                      pqxx::connection& connection);

    std::tuple<Status, std::string> getServerRoles(const std::string userId, 
            const std::string serverName, pqxx::connection& connection);

    void insertServerRoles(const std::string userID, const std::string serverName, 
            const Status statusEnum, optional<std::string> displayName,
            pqxx::connection& connection);

}

#endif //CHIT_CPP_SERVERFUNCS_H
