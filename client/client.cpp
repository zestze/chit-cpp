/*
 * client.cpp
 *
 * Zeke Reyna
 *
 * @TODO: revamp split method, use regex, or something.
 */

#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp> // not needed
#include <boost/algorithm/string.hpp>
#include <string>
#include <deque>
#include <array> // not needed
#include <tuple> // not needed
#include "user.h"
#include "client.h"

#include <unistd.h>
#include <pwd.h>
#include <stdio.h>
#include <limits.h>

#define 	BUFF_SIZE 		1024

using boost::asio::ip::tcp;

std::string RESERVED_CHARS[3] = {":", "!", "@"};

std::deque<std::string> sock_msgs;

bool DEBUG = false;
//bool DEBUG = true;

// grabbed from studiofreya.com
std::vector<std::string> split(std::string full_msg, std::string delim)
{
	std::vector<std::string> msgs;
	const auto npos = std::string::npos;
	const auto delim_size = delim.size();
	std::size_t offset = 0;
	std::size_t endpos = 0;
	std::size_t len = 0;

	do {
		endpos = full_msg.find(delim, offset);
		std::string temp;

		if (endpos != npos) {
			len = endpos - offset;
			temp = full_msg.substr(offset, len);
			msgs.push_back(temp);

			offset = endpos + delim_size;
		} else {
			temp = full_msg.substr(offset);
			msgs.push_back(temp);
			break;
		}
	} while (endpos != npos);
	return msgs;
}

void try_writing_to_sock(tcp::socket& sock, std::string msg)
{
	try {
		if (msg.substr(msg.length() - 2, std::string::npos) != "\r\n")
			throw std::invalid_argument("All IRC msgs need \\r\\n suffix");
		boost::system::error_code ec;
		boost::asio::write(sock, boost::asio::buffer(msg),
				boost::asio::transfer_all(), ec);
		if (DEBUG)
			std::cout << "WRITE: " << msg << std::endl;
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

		std::array<char, BUFF_SIZE> buff = { };
		boost::system::error_code ec;
		sock.read_some(boost::asio::buffer(buff), ec);
		std::string full_msg(buff.data());
		std::vector<std::string> msgs;

		if (DEBUG)
			std::cout << "READ FULL_MSG: " << full_msg << std::endl;

		msgs = split(full_msg, "\r\n");
		for (auto it = msgs.begin(); it != msgs.end(); ++it) {
			if (*it != "") {
				sock_msgs.push_back(*it);
				if (DEBUG)
					std::cout << "msg[..] = " << *it << std::endl;
			}
		}

		// @TODO:
		// honestly can grab first thing from adding in for loop
		// don't add, just return. Will be smoother.
		std::string retval = sock_msgs.front();
		sock_msgs.pop_front();
		return retval;
	} catch (...) {
		throw;
	}
}

// basically try_read_from_sock without the popping
// @TODO: loop with read_some to check if len amount is read.
void update_sock_msgs(tcp::socket& sock)
{
	try {
		std::size_t len = sock.available();
		if (len <= 0) { // could just put == 0...
			return;
		}
		//std::vector<char> buff;
		std::array<char, BUFF_SIZE> buff = { };
		boost::system::error_code ec;
		sock.read_some(boost::asio::buffer(buff), ec);
		std::string full_msg(buff.data());
		std::vector<std::string> msgs;

		if (DEBUG)
			std::cout << "FULL_MSG: " << full_msg << std::endl;
		msgs = split(full_msg, "\r\n");
		for (auto it = msgs.begin(); it != msgs.end(); ++it) {
			if (*it != "") {
				sock_msgs.push_back(*it);
				if (DEBUG)
					std::cout << "msgs[..] = " << *it << std::endl;
			}
		}
	} catch (...) {
		throw;
	}
}

std::string to_cyan(std::string msg)
{
	return "\033[1;36m" + msg + "\033[0m";
}

std::string to_magenta(std::string msg)
{
	return "\033[1;35m" + msg + "\033[0m";
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

	struct passwd *pw;
	uid_t uid;
	uid = geteuid();
	pw = getpwuid(uid);
	std::string user(pw->pw_name);

	msg  = "What is your real name?\n";
	std::cout << to_cyan(msg);
	std::string real;
	std::cin >> real;

	return User(nick, user, real);
}

void pass_user_info_to_server(User this_user, tcp::socket& serv_sock)
{
	try {
		// send NICK
		std::string msg = "NICK " + this_user.get_nick() + "\r\n";
		try_writing_to_sock(serv_sock, msg);
		// write to socket

		// send USER; asterisks for ignored fields
		msg  = "USER " + this_user.get_user() + " * * :";
		msg += this_user.get_real() + "\r\n";
		try_writing_to_sock(serv_sock, msg);

		std::string reply = try_reading_from_sock(serv_sock);
		if (DEBUG) {
			std::cout << "DEBUG: should be confirmation message\n";
			std::cout << reply << std::endl;
		}
		// @TODO: check if reply has correct reply in it.
	} catch (...) {
		throw;
	}
}

std::string parse_topic_msg(std::string msg)
{
	std::string new_msg = "";
	std::vector<std::string> parts;
	parts = split(msg, ":");
	for (auto it = parts.begin(); it != parts.end(); ++it) {
		if (it == parts.begin())
			continue;
		if (it == --parts.end())
			new_msg += *it;
		else
			new_msg += *it + ":";
	}
	return new_msg;
}

// lazy... but they do the same thing.
std::string parse_user_list_msg(std::string msg)
{
	return parse_topic_msg(msg);
}

std::string connect_to_channel(tcp::socket& sock)
{
	try {
		std::string msg;
		msg  = "\n";
		msg += "##########################\n";
		msg += "What #channel would you like to join?\n";
		std::cout << to_cyan(msg);

		std::string channel;
		std::cin >> channel;
		if (channel.substr(0, 1) != "#")
			channel = "#" + channel;

		msg  = "JOIN " + channel + "\r\n";
		try_writing_to_sock(sock, msg);

		// wait for confirmation message from server
		std::string reply = try_reading_from_sock(sock);
		if (DEBUG) {
			std::cout << "DEBUG: should be confirmation message\n";
			std::cout << reply << std::endl;
		}

		msg  = "\n";
		msg += "##########################\n";
		msg += "Successfully connected to " + channel + "\n";
		std::cout << to_cyan(msg);

		// should get TOPIC
		reply = try_reading_from_sock(sock);
		if (DEBUG)
			std::cout << reply << std::endl;

		msg  = "\n";
		msg += "##########################\n";
		msg += channel + " Topic:\n";
		msg += parse_topic_msg(reply) + "\n";
		std::cout << to_cyan(msg);

		// should get LIST of users
		reply = try_reading_from_sock(sock);
		if (DEBUG)
			std::cout << reply << std::endl;

		msg  = "\n";
		msg += "##########################\n";
		msg += channel + " Users:\n";
		msg += parse_user_list_msg(reply) + "\n";
		std::cout << to_cyan(msg);

		// should get END OF NAMES
		reply = try_reading_from_sock(sock);
		if (DEBUG)
			std::cout << reply << std::endl;

		return channel;
	} catch (...) {
		throw;
	}
}

void parse_session_msg(std::string msg)
{
	std::string to_print;
	if (msg.find("PRIVMSG") != std::string::npos) {
		// :<nick>!<user>@<user-ip> PRIVMSG <channel> :<msg>
		std::size_t found = msg.find("!");
		std::string nick = msg.substr(1, found - 1); // should work...
		found = msg.find(":", 1);
		std::string priv_msg = msg.substr(found + 1, std::string::npos); // should work..
		to_print = nick + ": " + priv_msg + "\n";
		std::cout << to_magenta(to_print);
	} else if (msg.find("PART") != std::string::npos) {
		// :<nick>!<user>@<user-ip> PART <channel> [:<parting-msg>]
		// ignoring [:<parting-msg>] for now
		std::size_t found = msg.find("!");
		std::string nick = msg.substr(1, found - 1); // should work..
		found = msg.find("PART");
		std::string channel = msg.substr(found + 5, std::string::npos);
		to_print = nick + " LEFT CHANNEL " + channel + "\n";
		std::cout << to_magenta(to_print);
	} else if (msg.find("JOIN") != std::string::npos) {
		// <nick>!<user>@<user-ip> JOIN <channel>
		std::size_t found = msg.find("!");
		std::string nick = msg.substr(0, found - 1);
		found = msg.find("JOIN");
		std::string channel = msg.substr(found + 5, std::string::npos);
		to_print = nick + " JOINED CHANNEL " + channel + "\n";
		std::cout << to_magenta(to_print);
	} else {
		std::string reply  = "ERROR, unrecognized message or buffer overflow\n";
		reply += msg + "\n";
		std::cout << reply;
		std::cout << "MSG length: " << msg.size() << std::endl;
		throw std::invalid_argument("not parseable message or buffer overflow\n");
	}
}

// return true if need to quit
bool parse_user_input(tcp::socket& sock, std::string msg, std::string channel)
{
	if (msg == "") {
		return false;
	} else if (msg == "EXIT") {
		std::string part_msg = "PART " + channel + "\r\n";
		// ignoring :<part-msg>
		try_writing_to_sock(sock, part_msg);
		return true;
	} else if (msg == "HELP") {
		std::string to_client;
		to_client  = "options...\n";
		to_client += "EXIT: exit the client\n";
		to_client += "HELP: print this dialog\n";
		std::cout << to_cyan(to_client);
		return false;
	} else {
		std::string priv_msg;
		priv_msg  = "PRIVMSG " + channel;
		priv_msg += " :" + msg + "\r\n";
		try_writing_to_sock(sock, priv_msg);
		return false;
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

		pass_user_info_to_server(this_user, serv_sock);

		std::string channel = connect_to_channel(serv_sock);
		this_user.set_channel(channel);

		std::string msg;
		msg  = "\n";
		msg += "##########################\n";
		msg += "Type and press <ENTER> to send a message\n";
		msg += "Type EXIT and press <ENTER> to exit the client\n";
		msg += "EXIT<ENTER>\n";
		msg += "Type HELP and press <ENTER> to find out more\n";
		msg += "HELP<ENTER>\n";
		msg += "Have fun :)\n";
		std::cout << to_cyan(msg);

		for (;;) {
			update_sock_msgs(serv_sock);

			for (auto it = sock_msgs.begin(); it != sock_msgs.end();
									++it) {
				if (DEBUG)
					std::cout << "MSG: " << *it << std::endl;
				parse_session_msg(*it);
			}

			sock_msgs.clear();

			std::cout << to_cyan(this_user.get_nick() + ": ");
			//std::cin >> msg;
			getline(std::cin, msg);

			bool quit = parse_user_input(serv_sock, msg, this_user.get_chan());
			if (quit)
				break;
		}

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
