/*
 * main.cpp
 *
 * Zeke Reyna
 */
#include "Server.h"
#include <iostream>

#define DEFAULT_SERVER "testServer"

int main(int argc, char **argv)
{
	if (argc != 2) {
		std::cerr << "Usage: ./main <server-port>\n";
		return -1;
	}
	int port = std::stoi(argv[1]);

    //@TODO: update config.xml to have default names and such
	//@TODO: make it so that args can be passed for servername
	Server serv(DEFAULT_SERVER);
	serv.run(port);

	return 0;
}
