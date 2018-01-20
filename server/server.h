/*
 * server.h
 *
 * Zeke Reyna
 *
 * @TODO: give descriptions for each method
 */

#ifndef __SERVER_H__
#define __SERVER_H__

// includes, object and method definitions.
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <array>
#include <deque>
#include <mutex>
#include <thread>
#include <tuple>
#include <atomic>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>

#include "servlet.h"
#include "user.h"
#include "consts_globs_shared.h"

using boost::asio::ip::tcp;

std::vector<std::string> split(std::string full_msg, std::string delim);

/*
 */
//std::string try_reading_from_sock(tcp::socket& sock);
std::string try_reading(tcp::socket& sock);

/*
 */
//void try_writing_to_sock(tcp::socket& sock, std::string msg);
void try_writing(tcp::socket& sock, std::string msg);

/*
 */
User register_session(tcp::socket& sock);

/*
 */
std::string get_channel_name(tcp::socket& sock);

/*
 */

std::map<tcp::endpoint, std::deque<std::string>> end_msgs;

#endif
