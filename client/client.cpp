/*
 * client.cpp
 *
 * Zeke Reyna
 */

#include "client.h"

std::string RESERVED_CHARS[3] = {":", "!", "@"};

std::deque<std::string> sock_msgs;

bool DEBUG = false;
//bool DEBUG = true;

std::string try_reading(tcp::socket& sock)
{
	return try_reading_from_sock(sock, sock_msgs);
}

void try_writing(tcp::socket& sock, std::string msg)
{
	try_writing_to_sock(sock, msg);
}

void update(tcp::socket& sock)
{
	update_sockmsgs(sock, sock_msgs);
}

User query_and_create()
{
	std::string msg;
	msg = std::string("\n") // not sure why have to do this...
	    + "##########################\n"
	    + "Going to ask for user info...\n"
	    + "Note: these are reserved characters that cannot be used:\n"
	    + ": ! @\n\n"
	    + "What is your nickname?\n";
	std::cout << to_blue(msg);

	std::string nick;
	getline(std::cin, nick);

	struct passwd *pw;
	uid_t uid;
	uid = geteuid();
	pw = getpwuid(uid);
	std::string user(pw->pw_name);

	msg  = "What is your real name?\n";
	std::cout << to_blue(msg);
	std::string real;
	getline(std::cin, real);

	return User(nick, user, real);
}

void pass_user_info_to_server(User this_user, tcp::socket& serv_sock)
{
	// send NICK
	std::string msg = "NICK " + this_user.get_nick() + "\r\n";
	try_writing(serv_sock, msg);
	// write to socket

	// send USER; asterisks for ignored fields
	msg = "USER " + this_user.get_user() + " * * :"
	    + this_user.get_real() + "\r\n";
	try_writing(serv_sock, msg);

	std::string reply = try_reading(serv_sock);
	if (DEBUG) {
		std::cout << "DEBUG: should be confirmation message\n";
		std::cout << reply << "\n";
	}
	// @TODO: check if reply has correct reply in it.
}

std::string parse_topic_msg(std::string msg)
{
	std::string new_msg = "";
	std::deque<std::string> parts;
	parts = split_(msg, ":");
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
	std::string msg;
	msg = std::string("\n")
	    + "##########################\n"
	    + "What #channel would you like to join?\n";
	std::cout << to_blue(msg);

	std::string channel;
	getline(std::cin, channel);

	if (channel.substr(0, 1) != "#")
		channel = "#" + channel;

	msg  = "JOIN " + channel + "\r\n";
	try_writing(sock, msg);

	// wait for confirmation message from server
	std::string reply = try_reading(sock);
	if (DEBUG) {
		std::cout << "DEBUG: should be confirmation message\n";
		std::cout << reply << "\n";
	}

	msg = std::string("\n")
	    + "##########################\n"
	    + "Successfully connected to " + channel + "\n";
	std::cout << to_blue(msg);

	// should get TOPIC
	reply = try_reading(sock);
	if (DEBUG)
		std::cout << reply << "\n";

	msg = std::string("\n")
	    + "##########################\n"
	    + channel + " Topic:\n"
	    + parse_topic_msg(reply) + "\n";
	std::cout << to_blue(msg);

	// should get LIST of users
	reply = try_reading(sock);
	if (DEBUG)
		std::cout << reply << "\n";

	msg = std::string("\n")
	    + "##########################\n"
	    + channel + " Users:\n"
	    + parse_user_list_msg(reply) + "\n";
	std::cout << to_blue(msg);

	// should get END OF NAMES
	reply = try_reading(sock);
	if (DEBUG)
		std::cout << reply << "\n";

	return channel;
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
		std::string reply = "ERROR, unrecognized message or buffer overflow\n"
			          + msg + "\n";
		std::cout << reply;
		std::cout << "MSG length: " << msg.size() << "\n";
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
		try_writing(sock, part_msg);
		return true;
	} else if (msg == "HELP") {
		std::string to_client;
		to_client = std::string("options...\n")
		          + "EXIT: exit the client\n"
		          + "HELP: print this dialog\n";
		std::cout << to_blue(to_client);
		return false;
	} else {
		std::string priv_msg;
		priv_msg = "PRIVMSG " + channel
		         + " :" + msg + "\r\n";
		try_writing(sock, priv_msg);
		return false;
	}
}

int main(int argc, char **argv)
{
	if (argc != 3) {
		std::cout << "Usage: ./client <server-ip> <server-port>\n";
		return -1;
	}
	std::ios_base::sync_with_stdio(false);
	std::cout << "Starting client...\n";
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
		msg = std::string("\n")
		    + "##########################\n"
		    + "Type and press <ENTER> to send a message\n"
		    + "Type EXIT and press <ENTER> to exit the client\n"
		    + "EXIT<ENTER>\n"
		    + "Type HELP and press <ENTER> to find out more\n"
		    + "HELP<ENTER>\n"
		    + "Have fun :)\n";
		std::cout << to_blue(msg);

		bool quit = false;
		while (!quit) {
			update(serv_sock);

			for (auto& msg : sock_msgs)
				parse_session_msg(msg);
			sock_msgs.clear();

			std::cout << to_cyan(this_user.get_nick() + ": ");
			getline(std::cin, msg);

			quit = parse_user_input(serv_sock, msg, this_user.get_chan());
		}

	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << "\n";
	}
	catch (...)
	{
		std::cout << "Unrecognized error\n";
	}

	return 0;
}
