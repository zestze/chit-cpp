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
extern std::string RPL_WELCOME;
extern std::string RPL_TOPIC;
extern std::string RPL_NAMREPLY;
extern std::string RPL_ENDOFNAMES;

// *************** shared ****************
// for purpose of telling threads to exit
extern std::atomic<bool> killself;

// for purpose of passing info to child threads.
extern std::map<
		std::string,
		std::deque<
			std::tuple<User, std::deque<std::string>>
			>
		> chan_newusers;

extern std::deque<tcp::socket> global_socks;

extern std::mutex gl_lock; // for both the map and the deque

#endif
