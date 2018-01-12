/*
 * server.cpp
 *
 * Zeke Reyna
 *
 * @TODO: sock_io ops need proper timeout mechanisms so doesn't block forever
 * or make actually async
 *
 * @TODO: if using socket as key doesn't work, can use socket.rem_endpoint()!
 */

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <deque>
#include <mutex>
#include <thread>

#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>

#include "servlet.h"
#include "server.h"
#include "user.h"

//typedef std::map<tcp::socket, std::deque<std::string>> Dict;


//bool killself = false; @TODO: defined in servlet.h for the moment

std::string try_reading_from_sock(tcp::socket& sock)
{
	try {
		std::unique_lock<std::mutex> lock(msgs_lock);
		if (sock_msgs.count(sock) && !sock_msgs[sock].empty()) {
			std::string msg = sock_msgs[sock].front();
			sock_msgs[sock].pop_front();
			return msg;
		}

		std::vector<char> buff;
		boost::system::error_code ec;
		sock.read_some(boost::asio::buffer(buff), ec);
		std::string full_msg(buff.begin(), buff.end());
		std::vector<std::string> msgs;

		boost::algorithm::split(msgs, full_msg, boost::is_any_of("\r\n"));
		for (auto it = msgs.begin(); it != msgs.end(); ++it) {
			if (*it != "")
				sock_msgs[sock].push_back(*it);
		}

		// @TODO:
		// honestly can grab first thing from adding in for loop
		// don't add, just return. Will be smoother.
		std::string msg = sock_msgs[sock].front();
		sock_msgs[sock].pop_front();
		return msg;
	} catch (...) {
		throw;
	}
}

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

// pass sockets to be reading from?
// @TODO: read for all socks in ssock_msgs or pass a vector of socks and
// just read from thos...
void update_sock_msgs(std::vector<tcp::socket> socks)
{
	try {
		std::unique_lock<std::mutex> lock(msgs_lock);
		for (auto it = socks.begin(); it != socks.end(); ++it) {
			tcp::socket& sock = *it; // not sure if this will work...
			std::size_t len = sock.available();
			if (len <= 0) // could just put == 0...
				continue;
			std::vector<char> buff;
			boost::system::error_code ec;
			sock.read_some(boost::asio::buffer(buff), ec);
			std::string full_msg(buff.begin(), buff.end());
			std::vector<std::string> msgs;

			boost::algorithm::split(msgs, full_msg, boost::is_any_of("\r\n"));
			for (auto it2 = msgs.begin(); it2 != msgs.end(); ++it2) {
				if (*it2 != "")
					sock_msgs[sock].push_back(*it2);
			}
		}
	} catch (...) {
		throw;
	}
}

User register_session(tcp::socket& sock)
{
	try {
		std::string msg = try_reading_from_sock(sock);
		// "NICK <nick>"
		std::string nick = msg.substr(5, std::string::npos);

		msg = try_reading_from_sock(sock);
		// "USER <user-name> * * :<real-name>"
		std::deque<std::string> parts;
		boost::algorithm::split(parts, msg, boost::is_any_of(" * * :"));
		std::string part1, part2;
		part1 = parts.front();
		part2 = parts.back();
		std::string user_name, real_name;
		user_name = part1.substr(5, std::string::npos);
		real_name = part2;

		// @TODO: should User carry socket with it?
		User client(nick, user_name, real_name);
		// send "<this-IP> 001 <nick> :Welcome to the Internet
		// Relay Network <nick>!<user>@<their-IP>\r\n"
		std::string rem_IP = sock.remote_endpoint().address().to_string();
		std::string loc_IP = sock.local_endpoint().address().to_string();

		msg  = loc_IP + " " + RPL_WELCOME + " " + nick + " :Welcome to";
		msg += " the Internet Relay Network " + nick + "!" + user_name;
		msg += "@" + rem_IP + "\r\n";
		try_writing_to_sock(sock, msg);

		return client;
	} catch (...) {
		throw;
	}
}

std::string get_channel_name(tcp::socket& sock)
{
	try {
		std::string msg = try_reading_from_sock(sock);
		std::string channel = msg.substr(5, std::string::npos);
		return channel;
	} catch (...) {
		throw;
	}
}

void add_client_to_dict(std::string chan, User client)
{
	std::unique_lock<std::mutex> dict_lock(newusers_lock);
	if (chan_newusers.count(chan))
		chan_newusers[chan].push_back(client);
	else
		chan_newusers[chan] = {client};
}

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

			std::cout << "Got a connection at listening port\n";
			/*
			std::string msg = "hello\n";
			boost::system::error_code ec;
			boost::asio::write(sock, boost::asio::buffer(msg), ec);
			*/
			User client = register_session(sock);

			std::string channel = get_channel_name(sock);
			client.set_channel(channel);
			add_client_to_dict(channel, client);

			// @TODO: make servlet instance, pass it as arg in thread
			// @TODO: make thread, run it in servlet.cpp
		}
	}
	catch (const std::exception& e)
	{
		// @TODO: catch keyboard ctrl+c exception, clean up sockets and
		// exit. Or do regardless, for any exception.
		std::cout << e.what() << std::endl;
		// @TODO: let threads now they need to kill selves.
		// Should be an atomic change.
		killself = true;

		// @TODO: have a "join" waiting for all threads to finish.
		// thus, need a list keeping reference of all threads.
		return -1;
	}
	catch (...)
	{
		std::cout << "Unrecognized error" << std::endl;
		return -1;
	}
	return 0;
}
