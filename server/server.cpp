/*
 * server.cpp
 *
 * Zeke Reyna
 */

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "server.h"

using boost::asio::ip::tcp;

int main(int argc, char **argv)
{
	if (argc != 2) {
		std::cout << "Usage: ./server <server-port>" << std::endl;
		return -1;
	}
	std::cout << "Starting server..." << std::endl;
	int listen_port = std::stoi(argv[1]);

	try
	{
		boost::asio::io_service io_service;
		tcp::acceptor acceptor(io_service,
				tcp::endpoint(tcp::v4(), listen_port));
		// need to make events and threading support as well as catching ctrl+c
		for (;;) {
			tcp::socket sock(io_service);
			acceptor.accept(sock);

			/*
			std::string msg = "hello\n";
			boost::system::error_code ec;
			boost::asio::write(sock, boost::asio::buffer(msg), ec);
			*/
		}
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return -1;
	}
	catch (...)
	{
		std::cout << "Unrecognized error\n";
		return -1;
	}
	return 0;
}
