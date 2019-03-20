/*
 * main.cpp
 *
 * Zeke Reyna
 */
#include "Server.h"
#include <iostream>

int main(int argc, char **argv)
{
	if (argc != 2) {
		std::cerr << "Usage: ./main <server-port>\n";
		return -1;
	}
	int port = std::stoi(argv[1]);

	Server serv;
	serv.run(port);

	return 0;
}
