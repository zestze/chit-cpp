/*
 * client.h
 *
 * Zeke Reyna
 */

#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <User.h>
#include <sockio.h>

#include <iostream>
#include <asio.hpp>
#include <string>
#include <deque>
#include <memory>
#include <regex>
#include <pqxx/pqxx>

// for grabbing username
#include <unistd.h>
#include <pwd.h>
#include <cstdio>
#include <climits>
#include <chitter.h>
// for grabbing username

//using boost::asio::ip::tcp;
using tcp = asio::ip::tcp;

std::string const RESERVED_CHARS[3] = {":", "!", "@"};

/*
 */
std::string inline to_blue(const std::string &msg) {
	return "\033[1;34m" + msg + "\033[0m";
}

/*
 */
std::string inline to_cyan(const std::string &msg) {
	return "\033[1;36m" + msg + "\033[0m";
}

/*
 */
std::string inline to_magenta(const std::string &msg) {
	return "\033[1;35m" + msg + "\033[0m";
}

enum client_code { quitting, not_quitting, switching };

class Client {
	public:

        // register db connection on construction
        Client() :_dbConnection{chitter::initiate("../shared/config")} { }

        //@TODO: make constructors and destructors default and delete

		std::string try_reading();

		void try_writing(std::string msg);

		void update();

		// returns true if should continue, false if shouldn't
		bool query_and_create();

		void pass_user_info_to_server();

		std::string parse_topic_msg(std::string msg);

		std::string parse_user_list_msg(std::string msg);

		std::string connect_to_channel();

		void parse_session_msg(std::string msg);

		void handle_topic_request();

		client_code parse_user_input(std::string msg);

		void set_sock(asio::io_service& ios);

		void run(std::string serv_ip, std::string port);

	private:
		//tcp::socket _sock;
		std::unique_ptr<tcp::socket> _sockptr;
		User _user;

		std::deque<std::string> _sock_msgs;

		// channel related info
		std::string _channel_topic;
		std::string _channel_name;
};

#endif
