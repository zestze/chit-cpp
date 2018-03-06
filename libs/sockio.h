/*
 * sockio.h
 *
 * Zeke Reyna
 *
 * @TODO: implement timeout mechanism
 * @TODO: add error variables, to pass to calling function when socket fails
 * or is closed
 */

#ifndef __SOCKIO_H__
#define __SOCKIO_H__

#include <iostream>
#include <deque>
#include <array>
#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

namespace sockio {

/*
 */
std::deque<std::string> split(std::string full_msg, std::string delim);

/*
 */
std::string try_reading_from_sock(tcp::socket& sock, std::deque<std::string>& sock_msgs);

/*
 */
void try_writing_to_sock(tcp::socket& sock, std::string msg);

/*
 */
void update_sockmsgs(tcp::socket& sock, std::deque<std::string>& sock_msgs);

};

#endif
