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

#include "user.h"

#include <string>
#include <atomic>
#include <map>
#include <deque>
#include <mutex>
#include <tuple>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

// *************** constants ************
extern std::string RPL_WELCOME;
extern std::string RPL_TOPIC;
extern std::string RPL_NAMREPLY;
extern std::string RPL_ENDOFNAMES;

// *************** shared ****************
// for purpose of telling threads to exit
extern std::atomic<bool> killself;

#endif
