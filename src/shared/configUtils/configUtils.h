//
// Created by zeke on 3/20/19.
//

#ifndef CHIT_CPP_CONFIGUTILS_H
#define CHIT_CPP_CONFIGUTILS_H

#include <pugixml.hpp>

namespace configUtils {

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

    //@TODO: rename from default to serverName, serverTopic so tht it's
    // set in the xml config rather than given on the command line
    ///
    /// \struct ServerConfig
    /// \brief used to store all the config info for setting up a server
    struct ServerConfig {
        /// \brief the default name to use for the server
        std::string defaultName;
        /// \brief the default topic to use in the server
        std::string defaultTopic;
    };

    ///
    /// \brief convenience function for checking if a node is null
    /// \param node the node to check
    /// \throws exception if node is null
    void checkNode(const pugi::xml_node& node);

    ///
    /// \brief convenience function for performing common
    /// loading
    /// \param fileName the document to load
    /// \return the xml_node that represents the root of the config
    pugi::xml_node loadSetup(const std::string& fileName);

    ///
    /// \brief convenience function for grabbing the value of a given
    /// node and removing all the whitespace
    /// \param node the node which holds the text
    /// \return the string of the node's text value
    std::string toString(const pugi::xml_node& node);

    ///
    /// \brief for loading a config file into a local variable
    /// \param fileName the name of the config file
    /// \return the relevant config info stored in a struct
    DbConfig loadDbConfig(const std::string fileName = "config.xml");

    ///
    /// \brief for loading a config file into a local variable
    /// \param fileName the name of the config file
    /// \return the relevant config info stored in a struct
    ServerConfig loadServerConfig(const std::string fileName = "config.xml");

};

#endif //CHIT_CPP_CONFIGUTILS_H
