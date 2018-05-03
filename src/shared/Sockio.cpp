/*
 * sockio.cpp
 *
 * Zeke Reyna
 * @TODO: use std::regex for split_
 */
#include "Sockio.h"

//const int BUFF_SIZE = 1024;
#define BUFF_SIZE 1024

std::deque<std::string> sockio::split(std::string full_msg, std::string delim)
{
	std::deque<std::string> msgs;
	std::size_t offset = 0;
	std::size_t endpos = 0;
	std::size_t len    = 0;

	do {
		endpos = full_msg.find(delim, offset);
		std::string temp;

		if (endpos != std::string::npos) {
			len = endpos - offset;
			temp = full_msg.substr(offset, len);
			msgs.push_back(temp);

			offset = endpos + delim.size();
		} else {
			temp = full_msg.substr(offset);
			msgs.push_back(temp);
			break;
		}
	} while (endpos != std::string::npos);
	return msgs;
}

asio::error_code sockio::try_writing_to_sock(tcp::socket& sock, std::string msg)
{
	if (msg.substr(msg.length() - 2, std::string::npos) != "\r\n")
		throw std::invalid_argument("All IRC msgs need \\r\\n suffix");
	asio::error_code ec;
	asio::write(sock, asio::buffer(msg),
			asio::transfer_all(), ec);
	return ec;
	//if (DEBUG)
		//std::cout << "WRITE: " << msg << std::endl;
}

std::string sockio::try_reading_from_sock(tcp::socket& sock,
		std::deque<std::string>& sock_msgs)
{
	if (!sock_msgs.empty()) {
		std::string msg = sock_msgs.front();
		sock_msgs.pop_front();
		return msg;
	}

	std::array<char, BUFF_SIZE> buff = { };
	asio::error_code ec;
	sock.read_some(asio::buffer(buff), ec);
	std::string full_msg(buff.data());
	std::deque<std::string> msgs;

	//if (DEBUG)
		//std::cout << "READ FULL_MSG: " << full_msg << std::endl;

	msgs = split(full_msg, "\r\n");
	for (const std::string& msg : msgs) {
	    if (!msg.empty())
	    	sock_msgs.push_back(msg);
	}

	std::string msg = sock_msgs.front();
	sock_msgs.pop_front();
	return msg;
}

void sockio::update_sockmsgs(tcp::socket& sock,
		std::deque<std::string>& sock_msgs)
{
	std::size_t len = sock.available();
	if (len <= 0) { // could just put == 0 ...
		return;
	}

	std::array<char, BUFF_SIZE> buff = { };
	asio::error_code ec;
	sock.read_some(asio::buffer(buff), ec);
	std::string full_msg(buff.data());
	std::deque<std::string> msgs;

	//if (DEBUG)
		//std::cout << "UPD FULL_MSG: " << full_msg << std::endl;

	msgs = sockio::split(full_msg, "\r\n");
	for (const std::string& msg : msgs) {
		if (!msg.empty())
			sock_msgs.push_back(msg);
	}
}
