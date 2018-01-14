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
std::string RPL_WELCOME    = "001";
std::string RPL_TOPIC 	   = "332";
std::string RPL_NAMREPLY   = "252";
std::string RPL_ENDOFNAMES = "366";

// *************** shared ****************
// for purpose of telling threads to exit
std::atomic<bool> killself(false);

// for purpose of passing info to child threads.
std::map<
	std::string,
	std::deque<
		std::tuple<User, tcp::socket, std::deque<std::string>>
		>
	> chan_newusers;

std::mutex newusers_lock;

#endif
