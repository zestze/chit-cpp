/*
 * client.h
 *
 * Zeke Reyna
 */

#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "user.h"
#include "../libs/sockio.h"

#include <iostream>
#include <boost/asio.hpp>
#include <string>
#include <deque>

// for grabbing username
#include <unistd.h>
#include <pwd.h>
#include <stdio.h>
#include <limits.h>
// for grabbing username

using boost::asio::ip::tcp;

/*
 */
std::string try_reading(tcp::socket& sock);

/*
 */
void try_writing(tcp::socket& sock, std::string msg);

/*
 */
void update(tcp::socket& sock);

/*
 */
std::string inline to_cyan(std::string msg) {
	return "\033[1;36m" + msg + "\033[0m";
}

/*
 */
std::string inline to_magenta(std::string msg) {
	return "\033[1;35m" + msg + "\033[0m";
}

/*
 */
User query_and_create();

/*
 */
void pass_user_info_to_server(User this_user, tcp::socket& serv_sock);

/*
 */
std::string parse_topic_msg(std::string msg);

/*
 */
std::string parse_user_list_msg(std::string msg);

/*
 */
std::string connect_to_channel(tcp::socket& sock);

/*
 */
void parse_session_msg(std::string msg);

/*
 */
bool parse_user_input(tcp::socket& sock, std::string msg, std::string channel);

#endif
