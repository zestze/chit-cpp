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

std::map<tcp::socket, std::deque<std::string>> sock_msgs;
std::mutex msgs_lock; // for Dict sock_msgs

std::map<std::string, std::deque<User>> chan_newusers;
std::mutex newusers_lock;

#endif
