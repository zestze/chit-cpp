/*
 * server.h
 *
 * Zeke Reyna
 *
 * @TODO: give descriptions for each method
 */

#ifndef __SERVER_H__
#define __SERVER_H__

#include "consts_globs_shared.h"
#include "servlet.h"
#include "user.h"

#include <iostream>
#include <string>
#include <map>
#include <array>
#include <deque>
#include <mutex>
#include <thread>
#include <tuple>
#include <atomic>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

/*
 */
std::string try_reading(tcp::socket& sock);

/*
 */
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
