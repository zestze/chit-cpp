//
// Created by zeke on 11/4/18.
//

#ifndef CHIT_CPP_INITIATE_H
#define CHIT_CPP_INITIATE_H

#include <string>
#include <pqxx/connection>
#include <pqxx/pqxx>

namespace chitter {

    ///
    /// \struct DbConfig
    /// \brief used to store all the config info for connecting to a db
    struct DbConfig {
        /// \brief the type of database (postgres, etc)
        std::string type;
        /// \brief the username of the user to sign in as
        std::string username;
        /// \brief the password of the user to sign in as
        std::string password;
        /// \brief the ip to use to setup the connection
        std::string ip;
        /// \brief the port to connect on
        std::string port;
        /// \brief the name of the database on the server
        std::string name;
    };

    ///
    /// \brief for loading a config file into a local variable
    /// \param fileName the name of the config file
    /// \return the relevant config info stored in a struct
    DbConfig loadConfig(const std::string fileName = "./config");

    ///
    /// \brief for initiating a connection to a psql server
    /// \param fileName the name of the config file
    /// \return a connection variable to use for communicating with database
    pqxx::connection initiate(const std::string fileName = "./config");

    ///
    /// \brief for initiating a connection to a psql server
    /// \param dbConfig the config file info in struct form
    /// \return a connection variable to use for communicating with database
    pqxx::connection initiate(const DbConfig dbConfig);

    ///
    /// \brief queries the database for the current datetime
    /// \param connection to use for communicating with db
    /// \return the current datetime in string form
    std::string getCurrentDatetime(pqxx::connection& connection);

    ///
    /// \brief takes in a vector of strings, and formats them into
    /// quote encapsulated elements of a tuple
    /// \param ss the stream to write the elements to
    /// \param work useful for wrapping elements in quotes
    /// \param values a vector of the elements to pass to the stream
    void passValues(std::stringstream& ss, pqxx::work& work,
            std::vector<std::string> values);

    void printResult(const pqxx::result result);
}

#endif //CHIT_CPP_INITIATE_H
