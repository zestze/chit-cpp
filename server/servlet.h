/*
 * servlet.h
 *
 * Zeke Reyna
 */
#ifndef __SERVLET_H__
#define __SERVLET_H__

#include <string>
#include "server.h"

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

	private:
		std::string channel_name;
		std::string topic;
		//std::map<tcp::socket, User> sock_users; // not sure on using socket as key again.

		std::deque<User> users;
		// @TODO: carry remote endpoint with each user for map access
};

//void handle_newusers(Servlet servlet);

//std::map<tcp::socket, std::deque<std::string>> copy_sockmsgs(Servlet servlet);

//bool check_newusers(std::string chan);
//bool check_sockmsgs(Servlet servlet);

//void handle_sockmsgs(std::map<tcp::socket, std::deque<std::string>> local_sockmsgs,
		//Servlet servlet);
//void handle_msg(std::string msg, tcp::socket& sock, User user);

void run(Servlet servlet);

#endif
