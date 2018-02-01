/*
 * sockio.cpp
 *
 * Zeke Reyna
 * @TODO: use std::regex for split_
 */
#include "sockio.h"

//const int BUFF_SIZE = 1024;
#define BUFF_SIZE 1024

//using boost::asio::ip::tcp;

std::deque<std::string> split_(std::string full_msg, std::string delim)
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

void try_writing_to_sock(tcp::socket& sock, std::string msg)
{
	if (msg.substr(msg.length() - 2, std::string::npos) != "\r\n")
		throw std::invalid_argument("All IRC msgs need \\r\\n suffix");
	boost::system::error_code ec;
	boost::asio::write(sock, boost::asio::buffer(msg),
			boost::asio::transfer_all(), ec);
	//if (DEBUG)
		//std::cout << "WRITE: " << msg << std::endl;
}

std::string try_reading_from_sock(tcp::socket& sock, std::deque<std::string>& sock_msgs)
{
	if (!sock_msgs.empty()) {
		std::string msg = sock_msgs.front();
		sock_msgs.pop_front();
		return msg;
	}

	std::array<char, BUFF_SIZE> buff = { };
	boost::system::error_code ec;
	sock.read_some(boost::asio::buffer(buff), ec);
	std::string full_msg(buff.data());
	std::deque<std::string> msgs;

	//if (DEBUG)
		//std::cout << "READ FULL_MSG: " << full_msg << std::endl;

	msgs = split_(full_msg, "\r\n");
	for (auto it = msgs.begin(); it != msgs.end(); ++it) {
		if (*it != "") {
			sock_msgs.push_back(*it);
		}
	}

	std::string msg = sock_msgs.front();
	sock_msgs.pop_front();
	return msg;
}

void update_sockmsgs(tcp::socket& sock, std::deque<std::string>& sock_msgs)
{
	std::size_t len = sock.available();
	if (len <= 0) { // could just put == 0 ...
		return;
	}

	std::array<char, BUFF_SIZE> buff = { };
	boost::system::error_code ec;
	sock.read_some(boost::asio::buffer(buff), ec);
	std::string full_msg(buff.data());
	std::deque<std::string> msgs;

	//if (DEBUG)
		//std::cout << "UPD FULL_MSG: " << full_msg << std::endl;

	msgs = split_(full_msg, "\r\n");
	for (auto it = msgs.begin(); it != msgs.end(); ++it) {
		if (*it != "") {
			sock_msgs.push_back(*it);
		}
	}
}
