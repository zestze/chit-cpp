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
#include <csignal>
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

		// DON'T USE LET COMPILER GENERATE DEFAULT DESTRUCTOR
		/*
		~Servlet()
		{
		}
		*/

		// member modifiers
		void set_topic(std::string new_t) { _topic = new_t; }

		// member accessors
		std::string get_chan() { return _channel_name; }
		std::string get_topic() { return _topic; }
		void add_user(User new_u) { _users.push_back(new_u); }

		// formerly not member functions
		std::deque<tcp::socket>::iterator get_sock_for_user(User user);

		std::deque<User> grab_new();

		std::string try_reading(User user);

		void try_writing(User user, std::string msg);

		void update_endmsgs();

		void handle_newusers();

		bool check_user_in(User user, std::deque<User> deq);

		// return TRUE if needs handling
		bool check_newusers();

		// return TRUE if needs handling
		bool check_endmsgs();

		void handle_msg(std::string msg, tcp::endpoint end);

		void handle_endmsgs();

	private:
		std::map<tcp::endpoint, std::deque<std::string>> _end_msgs;
		std::deque<User> _users;
		std::deque<tcp::socket> _socks;
		std::string _channel_name;
		std::string _topic;
};

void run(std::string channel);

#endif
