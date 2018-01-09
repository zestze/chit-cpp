/*
 * client.cpp
 *
 * Zeke Reyna
 */

#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <string>
#include "user.h"
#include "client.h"

#include <unistd.h>
#include <limits.h>

using boost::asio::ip::tcp;

std::string RESERVED_CHARS[3] = {":", "!", "@"};

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
