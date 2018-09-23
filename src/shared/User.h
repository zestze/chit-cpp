//
// Created by zeke on 8/22/18.
//

#ifndef CHIT_CPP_USER_H
#define CHIT_CPP_USER_H

#include <string>
#include <utility>
#include <asio.hpp>
#include <iostream>

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
        _nick      = other._nick;
        _whoami = other._whoami;
        _real_name = other._real_name;
        _password  = other._password;
        _channel   = other._channel;
        _endpt     = other._endpt;
    }

    ~User() = default;

    void set_nick(const std::string new_nick) { _nick = new_nick; }
    void set_pass(const std::string new_pass) { _password = new_pass; }
    void set_channel(const std::string new_chan) { _channel = new_chan; }
    void set_endpoint(const tcp::endpoint new_e) { _endpt = new_e; }

    std::string get_nick() const { return _nick; }
    std::string get_real() const { return _real_name; }
    std::string get_pass() const { return _password; }
    std::string get_chan() const { return _channel; }
    std::string get_whoami()  const { return _whoami; }
    tcp::endpoint get_endpt() const { return _endpt; }

    std::string print_() const
    {
        std::stringstream msg;
        msg << "nick: "    << _nick      << '\n'
            << "whoami: "  << _whoami    << '\n'
            << "real: "    << _real_name << '\n'
            << "channel: " << _channel   << '\n';
        return msg.str();
    }

    friend std::ostream& operator << (std::ostream& os, User user) {
        os << "nick: " << user._nick << '\n';
        os << "whoami: " << user._whoami << '\n';
        os << "real: " << user._real_name << '\n';
        os << "channel: " << user._channel << '\n';
        return os;
    }

private:
    std::string _nick,
                _whoami,
                _real_name,
                _password,
                _channel;
    tcp::endpoint _endpt;

};

#endif //CHIT_CPP_USER_H
