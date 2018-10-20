//
// Created by zeke on 8/22/18.
//
//@TODO: think through which constructors and parameters we want to allow

#ifndef CHIT_CPP_USER_H
#define CHIT_CPP_USER_H

#include <string>
#include <utility>
#include <asio.hpp>
#include <iostream>
#include <status.h>

using tcp = asio::ip::tcp;

class User {
public:
    User() {
        _nick      = "";
        _whoami = "";
        _real_name = "";
        _password  = "";
        _channel   = "";
    }

    User(const std::string& n, const std::string& u, const std::string& r)
    :_nick{n}, _whoami{u}, _real_name{r} {
        _password = "";
        _channel  = "";
    }

    User(const std::string& n, const std::string& u, const std::string& r,
            const std::string& p)
            :_nick{n}, _whoami{u}, _real_name{r}, _password{p} {
        _channel  = "";
    }

    User(const User& other) {
        _nick         = other._nick;
        _whoami       = other._whoami;
        _real_name    = other._real_name;
        _password     = other._password;
        _channel      = other._channel;
        _endpt        = other._endpt;
        _server_role  = other._server_role;
        _channel_role = other._channel_role;
    }

    ~User() = default;

    void set_nick(const std::string new_nick) { _nick = new_nick; }
    void set_pass(const std::string new_pass) { _password = new_pass; }
    void set_channel(const std::string new_chan) { _channel = new_chan; }
    void set_endpoint(const tcp::endpoint new_e) { _endpt = new_e; }
    void set_server_role(const Status s_enum) { _server_role = s_enum; }
    void set_channel_role(const Status s_enum) { _channel_role = s_enum; }

    std::string get_nick() const { return _nick; }
    std::string get_real() const { return _real_name; }
    std::string get_pass() const { return _password; }
    std::string get_chan() const { return _channel; }
    std::string get_whoami()  const { return _whoami; }
    tcp::endpoint get_endpt() const { return _endpt; }
    Status get_server_role() const { return _server_role; }
    Status get_channel_role() const { return _channel_role; }

    std::string print_() const
    {
        std::stringstream msg;
        msg << "nick: "    << _nick      << '\n'
            << "whoami: "  << _whoami    << '\n'
            << "real: "    << _real_name << '\n'
            << "channel: " << _channel   << '\n'
            << "server_role: " << _server_role << '\n'
            << "channel_role: " << _channel_role << '\n';
        return msg.str();
    }

    friend std::ostream& operator << (std::ostream& os, User user) {
        os << "nick: " << user._nick << '\n';
        os << "whoami: " << user._whoami << '\n';
        os << "real: " << user._real_name << '\n';
        os << "channel: " << user._channel << '\n';
        os << "server_role: " << user._server_role << '\n';
        os << "channel_role: " << user._channel_role << '\n';
        return os;
    }

private:
    std::string _nick,
                _whoami,
                _real_name,
                _password,
                _channel;
    Status      _server_role,
                _channel_role;
    tcp::endpoint _endpt;

};

#endif //CHIT_CPP_USER_H
