/*
 * main.cpp
 *
 * Zeke Reyna
 */
#include "client.h"

int main(int argc, char **argv)
{
	if (argc != 3) {
		std::cerr << "Usage: ./client <server-ip> <server-port>\n";
		return -1;
	}

	std::string ip (argv[1]);
	std::string port (argv[2]);

	Client client;
	client.run(ip, port);

	return 0;
}
