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
#include <array>
#include <map>
#include <boost/algorithm/string.hpp>

#include "user.h"
#include "consts_globs_shared.h"

using boost::asio::ip::tcp;

class Servlet {
	public:
		// DON'T USE
		Servlet()
		{
			_channel_name = "";
			_topic = "DEFAULT TOPIC";
		}

		Servlet(std::string c)
			:_channel_name{c}
		{
			_topic = "DEFAULT TOPIC";
		}

		Servlet(Servlet& other)
		{
			_channel_name = other._channel_name;
			_topic = other._topic;
		}

		// DON'T USER
		~Servlet()
		{
		}

		// associated functions.
		//void run();

		//void handle_newusers();

		// member modifiers
		void set_topic(std::string new_t) { _topic = new_t; }

		// member accessors
		std::string get_chan() { return _channel_name; }
		std::string get_topic() { return _topic; }
		//std::map<tcp::socket, User> get_dict() { return sock_users; }
		void add_user(User new_u) { users.push_back(new_u); }

		//std::map<tcp::endpoint, tcp::socket> end_socks; // @TODO: make private and make public functions

		std::map<tcp::endpoint, std::deque<std::string>> end_msgs;

		std::deque<User> users;

		std::deque<tcp::socket> socks;

	private:
		std::string _channel_name;
		std::string _topic;
};

std::vector<std::string> split_(std::string full_msg, std::string delim);

// returns iterator
std::deque<tcp::socket>::iterator get_sock_for_user(Servlet& srvlt, User usr);

std::deque<User> grab_new(Servlet& servlet);

/*
 */
std::string try_reading(Servlet& servlet, User user);

/*
 */
//void try_writing(tcp::socket& sock, std::string msg);
void try_writing(Servlet& srvlt, User usr, std::string msg);

/*
 */
//std::deque<tcp::endpoint> update_end_msgs(Servlet& servlet);
void update_end_msgs(Servlet& servlet);

/*
 * modifies servlet.users, and while grabbing users adds them
 */
//std::deque<User> grab_newusers(Servlet& servlet);

//void get_newsockets(Servlet& srvlt);

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
//void run(Servlet servlet);
void run(std::string channel);

#endif
