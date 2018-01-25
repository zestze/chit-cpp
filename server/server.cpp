/*
 * server.cpp
 *
 * Zeke Reyna
 *
 * @TODO: sock_io ops need proper timeout mechanisms so doesn't block forever
 * or make actually async
 *
 * @TODO: still need to implement proper graceful exit. Use a signal handler
 * for CTRL+C? Or can wait for input such as "QUIT" or "EXIT"
 */

#include "server.h"
#include "../libs/sockio.h"

// ************ GLOBALS ****************
std::string RPL_WELCOME;
std::string RPL_TOPIC;
std::string RPL_NAMREPLY;
std::string RPL_ENDOFNAMES;

std::atomic<bool> killself;

std::map<std::string, std::deque<std::tuple<User, std::deque<std::string>>>> chan_newusers;
std::deque<tcp::socket> global_socks;
std::mutex gl_lock;
// ************ GLOBALS ****************

// UNFINISHED
void signal_handler(int signal)
{
	if (signal)
		killself = true;
}

std::string try_reading(tcp::socket& sock)
{
	tcp::endpoint end = sock.remote_endpoint();
	if (!end_msgs.count(end))
		end_msgs[end]; // instantiate bc i'm nervous
	std::deque<std::string>& sock_msgs = end_msgs[end];
	return try_reading_from_sock(sock, sock_msgs);
}

void try_writing(tcp::socket& sock, std::string msg)
{
	try_writing_to_sock(sock, msg);
}

User register_session(tcp::socket& sock)
{
	std::string msg = try_reading(sock);
	// "NICK <nick>"
	std::string nick = msg.substr(5, std::string::npos);

	msg = try_reading(sock);
	// "USER <user-name> * * :<real-name>"
	std::cout << "MSG: " << msg << std::endl;
	std::deque<std::string> parts = split_(msg, " * * :");

	for (auto it = parts.begin(); it != parts.end(); ++it) {
		std::cout << "parts[..] = " << *it << std::endl;
	}
	std::string part1, part2;
	part1 = parts.front();
	part2 = parts.back();
	std::string user_name, real_name;
	user_name = part1.substr(5, std::string::npos);
	real_name = part2;

	User client(nick, user_name, real_name);
	// send "<this-IP> 001 <nick> :Welcome to the Internet
	// Relay Network <nick>!<user>@<their-IP>\r\n"
	std::string rem_IP = sock.remote_endpoint().address().to_string();
	std::string loc_IP = sock.local_endpoint().address().to_string();

	msg  = loc_IP + " " + RPL_WELCOME + " " + nick + " :Welcome to";
	msg += " the Internet Relay Network " + nick + "!" + user_name;
	msg += "@" + rem_IP + "\r\n";
	try_writing(sock, msg);

	return client;
}

std::string get_channel_name(tcp::socket& sock)
{
	std::string msg = try_reading(sock);
	std::string channel = msg.substr(5, std::string::npos);
	return channel;
}

void set_globals()
{
	RPL_WELCOME    = "001";
	RPL_TOPIC      = "332";
	RPL_NAMREPLY   = "252";
	RPL_ENDOFNAMES = "366";

	killself = false;
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		std::cout << "Usage: ./server <server-port>" << std::endl;
		return -1;
	}
	std::cout << "Starting server..." << std::endl;
	int listen_port = std::stoi(argv[1]);

	set_globals();

	std::map<std::string, std::thread> threads;
	try
	{
		boost::asio::io_service io_service;
		tcp::acceptor acceptor(io_service,
				tcp::endpoint(tcp::v4(), listen_port));

		for (;;) {
			tcp::socket sock(io_service);
			acceptor.accept(sock);

			std::cout << "Got a connection at listening port\n";
			std::cout << "Going to register User\n";
			User client = register_session(sock);
			client.set_endpoint(sock.remote_endpoint());

			std::cout << "Going to get channel pref\n";
			std::string channel = get_channel_name(sock);
			client.set_channel(channel);

			client.set_endpoint(sock.remote_endpoint());

			std::deque<std::string> temp_msgs;
			for (auto msg = end_msgs[sock.remote_endpoint()].begin();
					msg != end_msgs[sock.remote_endpoint()].end();
					++msg) {
				temp_msgs.push_back(*msg);
			}
			end_msgs.erase(sock.remote_endpoint());

			{
				std::unique_lock<std::mutex> lck(gl_lock);
				chan_newusers[channel].push_back(std::make_tuple(client,
							temp_msgs));
				global_socks.push_back(std::move(sock));
			}

			if (!threads.count(channel)) {
				std::thread thr(run, channel);
				threads[channel] = std::move(thr);
			}
		}
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;

		killself = true; // atomic
		for (auto it = threads.begin(); it != threads.end(); ++it)
			it->second.join(); // might not work??? it's a pointer so maybe... but no copy constr.
		return -1;
	}
	catch (...)
	{
		std::cout << "Unrecognized error" << std::endl;
		killself = true;
		for (auto it = threads.begin(); it != threads.end(); ++it)
			it->second.join();
		return -1;
	}
	return 0;
}
