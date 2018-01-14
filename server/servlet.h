/*
 * servlet.h
 *
 * Zeke Reyna
 *
 * @TODO: give descriptions for each function
 */
#ifndef __SERVLET_H__
#define __SERVLET_H__

#include <string>
#include <iostream>
#include <deque>
#include <map>
#include <boost/algorithm/string.hpp>

#include "user.h"
#include "consts_globs_shared.h"

using boost::asio::ip::tcp;

class Servlet {
	public:
		Servlet();

		Servlet(std::string c)
			:channel_name{c}
		{
			topic = "DEFAULT TOPIC";
		}

		~Servlet();

		// associated functions.
		//void run();

		//void handle_newusers();

		// member modifiers
		void set_topic(std::string new_t) { topic = new_t; }

		// member accessors
		std::string get_chan() { return channel_name; }
		std::string get_topic() { return topic; }
		//std::map<tcp::socket, User> get_dict() { return sock_users; }
		void add_user(User new_u) { users.push_back(new_u); }

		std::map<tcp::endpoint, tcp::socket> end_socks; // @TODO: make private and make public functions

		std::map<tcp::endpoint, std::deque<std::string>> end_msgs;

		std::deque<User> users;

	private:
		std::string channel_name;
		std::string topic;
};

/*
 */
std::string try_reading(Servlet& servlet, User user);

/*
 */
void try_writing(tcp::socket& sock, std::string msg);

/*
 */
std::deque<tcp::endpoint> update_end_msgs(Servlet& servlet);

/*
 * modifies servlet.users, and while grabbing users adds them
 */
std::deque<User> grab_newusers(Servlet& servlet);

/*
 */
void handle_newusers(Servlet& servlet);

/*
 */
bool check_newusers(std::string chan);

/*
 */
bool check_end_msgs(Servlet& servlet);

/*
 */
void handle_msg(std::string msg, Servlet& servlet, tcp::endpoint end);

/*
 */
void run(Servlet servlet);

#endif
