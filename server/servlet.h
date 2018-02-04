/*
 * servlet.h
 *
 * Zeke Reyna
 *
 * The servlet is a thread that handles each chat channel.
 * It can hold indeterminate amounts of users, and handle users joining, messaging,
 * and leaving channel. Not much else yet.
 * _end_msgs is a map with an endpoint as the key since using a socket is impossible,
 * and if possible is wasteful. Users carry endpoints with them, so that the deque
 * of messages for the user can be indexed by their endpoints.
 *
 * @TODO: make retvals and args const where possible
 * @TODO: update try_reading and try_writing to account for broken pipes
 * @TODO: implement PriorityQueue for end_msgs to account for PART messages
 * @TODO: write a log of chat history to a named file
 * @TODO: give descriptions for each function
 * @TODO: update unfinished methods
 *
 * @TODO: there seems to be a std bad_alloc that occurs when last user leaves
 * the chat. Might be vectors or something getting pushed on the stack? Wrap
 * all functions in try / catch and print a psuedo stack trace
 */
#ifndef __SERVLET_H__
#define __SERVLET_H__

#include "user.h"
#include "consts_globs_shared.h"

#include <string>
#include <iostream>
#include <deque>
#include <array>
#include <map>
#include <csignal>
#include <boost/algorithm/string.hpp>

using boost::asio::ip::tcp;

class Servlet {
	public:
		/*
		 * UNFINISHED
		 */
		Servlet()
		{
			_channel_name = "";
			_topic = "DEFAULT TOPIC";
		}

		/*
		 * UNFINISHED
		 */
		Servlet(std::string c)
			:_channel_name{c}
		{
			_topic = "DEFAULT TOPIC";
		}

		/*
		 * This looks gross.
		 */
		Servlet(std::string c,
				std::map<std::string, std::deque<
				std::tuple<User, std::deque<std::string>>
				>> *cnu_ptr,
				std::deque<tcp::socket> *gs_ptr,
				std::mutex *gl_ptr)
			:_chan_newusers_ptr{cnu_ptr}, _global_socks_ptr{gs_ptr},
			_gl_lock_ptr{gl_ptr}, _channel_name{c}
		{
			_topic = "DEFAULT TOPIC";
		}

		/*
		 * SHOULD ONLY BE USED BFORE ADDING ANY USERS OR SOCKETS
		 */
		Servlet(Servlet& other)
		{
			_channel_name = other._channel_name;
			_topic = other._topic;
		}

		/*
		 */
		// DON'T USE LET COMPILER GENERATE DEFAULT DESTRUCTOR
		/*
		~Servlet()
		{
		}
		*/

		// member modifiers
		//void set_topic(std::string new_t) { _topic = new_t; }

		// member accessors
		// kind of deprecated...
		/*
		std::string get_chan() { return _channel_name; }
		std::string get_topic() { return _topic; }
		void add_user(User new_u) { _users.push_back(new_u); }
		*/

		/*
		 */
		std::deque<tcp::socket>::iterator get_sock_for_user(User user);

		/*
		 * Grab the newusers waiting to join channel and the messages
		 * that they've passed along thus far. After grabbing them from
		 * the global deque, grab the sockets associated with them
		 * as well.
		 */
		std::deque<User> grab_new();

		/*
		 * Wrapper around sockio's try_reading_from_sock(...)
		 * Since sockets are OS resources and thus not copy constructable,
		 * are only ever held in a global deque. So need to grab user's
		 * associated socket before reading info from user.
		 */
		std::string try_reading(User user);

		/*
		 * Wrapper around sockio's try_writing_to_sock(...)
		 * Since sockets are OS resources and thus not copy constructable,
		 * are only ever held in a global deque. So need to grab user's
		 * associated socket before writing info to user.
		 */
		void try_writing(User user, std::string msg);

		/*
		 * Read all the messages that users have sent to server, and move
		 * it into _end_msgs for later handling
		 */
		void update_endmsgs();

		/*
		 * After grabbing newusers, handle them.
		 * This involves notifying other users that <newuser> has joined
		 * the channel, and sending the <newuser> a couple of messages.
		 * Concerning the channal topic, the current list of users in it,
		 * and an 'end' message.
		 */
		void handle_newusers();

		/*
		 */
		bool check_user_in(User& user, std::deque<User>& deq);

		/*
		 * Check if there are newusers in global map that are waiting to
		 * join this chat.
		 * Return TRUE if so,
		 * Return FALSE otherwise.
		 */
		bool check_newusers();

		/*
		 * Check if there are messages in _end_msgs that need handling.
		 * Return TRUE if so,
		 * Return FALSE otherwise.
		 */
		// return TRUE if needs handling
		bool check_endmsgs();

		/*
		 * Check if message is a PRIVMSG, or a PART-ing message.
		 * if PRIVMSG, pass message along to other users in channel.
		 * if PART, cleanup resources associated with that user, and
		 * notify rest of channel that the user has left.
		 */
		void handle_msg(std::string msg, tcp::endpoint end);

		/*
		 * Go through every message of _end_msgs for every user,
		 * and handle them.
		 */
		void handle_endmsgs();

	private:
		// for comms with main thread
		std::map<std::string, std::deque<std::tuple<User, std::deque<std::string>
			>>> *_chan_newusers_ptr;
		std::deque<tcp::socket> *_global_socks_ptr;
		std::mutex *_gl_lock_ptr;
		// for comms with main thread


		std::map<tcp::endpoint, std::deque<std::string>> _end_msgs;
		std::deque<User> _users;
		std::deque<tcp::socket> _socks;
		std::string _channel_name;
		std::string _topic;
};

/*
 */
void run(std::string channel,
		std::map<std::string, std::deque<std::tuple<User, std::deque<std::string>>
		>> *chan_newusers_ptr,
		std::deque<tcp::socket> *global_socks_ptr,
		std::mutex *gl_lock_ptr);
#endif
