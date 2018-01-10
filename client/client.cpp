/*
 * client.cpp
 *
 * Zeke Reyna
 */

#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp> // not needed
#include <boost/algorithm/string.hpp>
#include <string>
#include <deque>
#include <array>
#include "user.h"
#include "client.h"

#include <unistd.h>
#include <limits.h>

using boost::asio::ip::tcp;

std::string RESERVED_CHARS[3] = {":", "!", "@"};

std::deque<std::string> sock_msgs;

void try_writing_to_sock(tcp::socket& sock, std::string msg)
{
	try {
		if (msg.substr(msg.length() - 2, std::string::npos) != "\r\n")
			throw std::invalid_argument("All IRC msgs need \\r\\n suffix");
		boost::system::error_code ec;
		boost::asio::write(sock, boost::asio::buffer(msg),
				boost::asio::transfer_all(), ec);
		// @TODO: implement proper async write, or have a timeout.
		// this blocks until all in buffer is transmitted.
	} catch (...) {
		throw;
	}
}

// @TODO: needs proper timeout mechanism.
std::string try_reading_from_sock(tcp::socket& sock)
{
	try {
		if (!sock_msgs.empty()) {
			std::string retval = sock_msgs.front();
			sock_msgs.pop_front();
			return retval;
		}

		//std::string full_msg;
		//boost::array<char, 128> buff;
		//std::array<char, 128> buff;
		//std::size_t len = sock.read_some(boost::asio::buffer(buff), ec);
		std::vector<char> buff;
		boost::system::error_code ec;
		sock.read_some(boost::asio::buffer(buff), ec);
		std::string full_msg(buff.begin(), buff.end());
		std::vector<std::string> msgs;

		boost::algorithm::split(msgs, full_msg, boost::is_any_of("\r\n"));
		for (auto it = msgs.begin(); it != msgs.end(); ++it) {
			if (*it != "")
				sock_msgs.push_back(*it);
		}

		std::string retval = sock_msgs.front();
		sock_msgs.pop_front();
		return retval;
	} catch (...) {
		throw;
	}
}

std::string to_cyan(std::string msg)
{
	return "\033[1;36m" + msg + "\033[0m";
}

User query_and_create()
{
	std::string msg;
	msg  = "\n";
	msg += "##########################\n";
	msg += "Going to ask for user info...\n";
	msg += "Note: these are reserved characters that cannot be used:\n";
	msg += ": ! @\n\n";
	msg += "What is your nickname?\n";
	std::cout << to_cyan(msg);

	std::string nick;
	std::cin >> nick;

	char user_name[LOGIN_NAME_MAX];
	getlogin_r(user_name, LOGIN_NAME_MAX);
	std::string user(user_name);

	msg  = "What is your real name?\n";
	std::cout << to_cyan(msg);
	std::string real;
	std::cin >> real;

	return User(nick, user, nick);
}

void pass_user_into_to_server(User this_user, tcp::socket& serv_sock)
{
	try {
		// send NICK
		std::string msg = "NICK " + this_user.nick + "\r\n";
		try_writing_to_sock(serv_sock, msg);
		// write to socket

		// send USER; asterisks for ignored fields
		msg  = "USER " + this_user.user_name + " * * :";
		msg += this_user.real_name + "\r\n";
		try_writing_to_sock(serv_sock, msg);

		std::string reply = try_reading_from_sock(serv_sock);
		// @TODO: check if reply has correct reply in it.
	} catch (...) {
		throw;
	}
}

int main(int argc, char **argv)
{
	if (argc != 3) {
		std::cout << "Usage: ./client <server-ip> <server-port>";
		std::cout << std::endl;
		return -1;
	}
	std::cout << "Starting client..." << std::endl;
	std::string serv_ip = argv[1];
	int serv_port = std::stoi(argv[2]);

	// might need to add support for strings other than localhost
	// str::string is unnecessary
	if (serv_ip == "localhost")
		serv_ip = "127.0.0.1";

	try
	{
		User this_user = query_and_create();

		boost::asio::io_service io_service;
		tcp::endpoint endpoint(
				boost::asio::ip::address_v4::from_string(serv_ip),
				serv_port);
		tcp::socket serv_sock(io_service);
		// @TODO: use RAII to have this socket close on destruction
		// although, tbh I think that boost sockets close connection on destruction.

		serv_sock.connect(endpoint);

		/*
		boost::array<char, 128> buff;
		boost::system::error_code ec;

		size_t len = serv_sock.read_some(boost::asio::buffer(buff), ec);
		std::cout.write(buff.data(), len);
		*/

		pass_user_into_to_server(this_user, serv_sock);

	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "Unrecognized error" << std::endl;
	}

	return 0;
}
