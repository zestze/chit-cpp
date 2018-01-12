/*
 * server.h
 *
 * Zeke Reyna
 */

#ifndef __SERVER_H__
#define __SERVER_H__

// includes, object and method definitions.
#include <string>
#include <map>
#include <mutex>
#include <shared_mutex>
#include <deque>

#include "user.h"

#include <boost/asio.hpp>

//@TODO: move these constants over to servlet.h or some shared 'h' file
//since server.h will need to include servlet.h at some point.

bool killself = false; // make an atomic variable?

using boost::asio::ip::tcp;

// *************** constants ************
std::string RPL_WELCOME    = "001";
std::string RPL_TOPIC 	   = "332";
std::string RPL_NAMREPLY   = "252";
std::string RPL_ENDOFNAMES = "366";

std::map<std::string, std::deque<User>> chan_newusers;
std::mutex newusers_lock;

std::map<tcp::endpoint, std::deque<std::string>> end_msgs;
std::mutex msgs_lock;
//@TODO: only way to fix this is by making end_msgs local to each servlet...

std::map<tcp::endpoint, tcp::socket> end_sock; //@TODO: better name?
std::mutex endsock_lock;
//std::shared_mutex end_sock_lock; // @TODO: R/W lock would be best here
// @TODO: does this need a lock?

#endif
