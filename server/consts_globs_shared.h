/*
 * consts_globs_shared.h
 *
 * Holds all the global constants, structures, and functions
 * that should be included in server.h and servlet.h
 *
 * Zeke Reyna
 */
#ifndef __CONSTS_GLOBS_SHARED_H__
#define __CONSTS_GLOBS_SHARED_H__

#include <string>
#include <atomic>
#include <map>
#include <deque>
#include <mutex>
#include <tuple>
#include <boost/asio.hpp>

#include "user.h"

using boost::asio::ip::tcp;

// *************** constants ************
//std::string RPL_WELCOME    = "001";
extern std::string RPL_WELCOME;
//std::string RPL_TOPIC 	   = "332";
extern std::string RPL_TOPIC;
//std::string RPL_NAMREPLY   = "252";
extern std::string RPL_NAMREPLY;
//std::string RPL_ENDOFNAMES = "366";
extern std::string RPL_ENDOFNAMES;

// *************** shared ****************
// for purpose of telling threads to exit
//std::atomic<bool> killself(false);
extern std::atomic<bool> killself;

// for purpose of passing info to child threads.
/*
std::map<
	std::string,
	std::deque<
		std::tuple<User, tcp::socket, std::deque<std::string>>
		>
	> chan_newusers;
	*/

extern std::map<
		std::string,
		std::deque<
			std::tuple<User, std::deque<std::string>>
			>
		> chan_newusers;

extern std::deque<tcp::socket> global_socks;
//std::mutex newusers_lock;

extern std::mutex gl_lock; // for both the map and the deque
//extern std::mutex c_nu_lock; // for chan_newusers
//extern std::mutex g_s_lock; // for global_socks
// lock for both chan_newusers and global_socks.

#endif
