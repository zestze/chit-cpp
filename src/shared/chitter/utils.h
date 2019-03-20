//
// Created by zeke on 11/4/18.
//

#ifndef CHIT_CPP_INITIATE_H
#define CHIT_CPP_INITIATE_H

#include <string>
#include <pqxx/connection>
#include <pqxx/pqxx>

#include "configUtils/configUtils.h"

namespace chitter {

    ///
    /// \brief for initiating a connection to a psql server
    /// \param fileName the name of the config file
    /// \return a connection variable to use for communicating with database
    pqxx::connection initiate(const std::string fileName = "config.xml");

    ///
    /// \brief for initiating a connection to a psql server
    /// \param dbConfig the config file info in struct form
    /// \return a connection variable to use for communicating with database
    pqxx::connection initiate(const configUtils::DbConfig dbConfig);

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
