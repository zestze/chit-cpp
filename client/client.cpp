/*
 * client.cpp
 *
 * Zeke Reyna
 *
 * @TODO: fix parse_session_msg, works fine, but handling is inconsistent.
 * @TODO: change to regex handling for finer handling
 * @TODO: move each parse_session_msg option into its own function
 */

#include "client.h"


bool DEBUG = false;
//bool DEBUG = true;

std::string Client::try_reading()
{
	return try_reading_from_sock(*_sockptr, _sock_msgs);
}

void Client::try_writing(std::string msg)
{
	try_writing_to_sock(*_sockptr, msg);
}

void Client::update()
{
	update_sockmsgs(*_sockptr, _sock_msgs);
}

void Client::query_and_create()
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

	_user = User(nick, user, real);
}

void Client::pass_user_info_to_server()
{
	// send NICK
	std::string msg = "NICK " + _user.get_nick() + "\r\n";
	try_writing(msg);
	// write to socket

	// send USER; asterisks for ignored fields
	msg = "USER " + _user.get_user() + " * * :"
	    + _user.get_real() + "\r\n";
	try_writing(msg);

	std::string reply = try_reading();
	if (DEBUG) {
		std::cout << "DEBUG: should be confirmation message\n";
		std::cout << reply << "\n";
	}
	// @TODO: check if reply has correct reply in it.
}

std::string Client::parse_topic_msg(std::string msg)
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
std::string Client::parse_user_list_msg(std::string msg)
{
	return parse_topic_msg(msg);
}

std::string Client::connect_to_channel()
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
	try_writing(msg);

	// wait for confirmation message from server
	std::string reply = try_reading();
	if (DEBUG) {
		std::cout << "DEBUG: should be confirmation message\n";
		std::cout << reply << "\n";
	}

	msg = std::string("\n")
	    + "##########################\n"
	    + "Successfully connected to " + channel + "\n";
	std::cout << to_blue(msg);

	// should get TOPIC
	reply = try_reading();
	if (DEBUG)
		std::cout << reply << "\n";

	_channel_topic = parse_topic_msg(reply);

	msg = std::string("\n")
	    + "##########################\n"
	    + channel + " Topic:\n"
	    + _channel_topic + "\n";
	std::cout << to_blue(msg);

	// should get LIST of users
	reply = try_reading();
	if (DEBUG)
		std::cout << reply << "\n";

	msg = std::string("\n")
	    + "##########################\n"
	    + channel + " Users:\n"
	    + parse_user_list_msg(reply) + "\n";
	std::cout << to_blue(msg);

	// should get END OF NAMES
	reply = try_reading();
	if (DEBUG)
		std::cout << reply << "\n";

	return channel;
}

void Client::parse_session_msg(std::string msg)
{
	//@TODO: change to regular expression matching to be robust
	//@TODO: :<nick> vs <nick> for begin of msg... inconsistent, so fix
	// bc msg.substr(... found - 1) vs msg.substr(... found)

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
		std::string nick = msg.substr(0, found);
		found = msg.find("JOIN");
		std::string channel = msg.substr(found + 5, std::string::npos);
		to_print = nick + " JOINED CHANNEL " + channel + "\n";
		std::cout << to_magenta(to_print);

	} else if (msg.find("TOPIC") != std::string::npos) {
		// <nick>!<user>@<user-ip> TOPIC <channel> :<new-topic>

		std::size_t found = msg.find("!");
		std::string nick = msg.substr(0, found);
		std::string delim = " TOPIC " + _channel_name + " :";
		found = msg.find(delim);
		std::string new_topic = msg.substr(found + delim.size(), std::string::npos);

		_channel_topic = new_topic;

		to_print = nick + " CHANGED TOPIC TO: " + _channel_topic + "\n";
		std::cout << to_magenta(to_print);
	} else {
		std::string reply = "ERROR, unrecognized message or buffer overflow\n"
			          + msg + "\n";
		std::cout << reply;
		std::cout << "MSG length: " << msg.size() << "\n";
		throw std::invalid_argument("not parseable message or buffer overflow\n");
	}
}

void Client::handle_topic_request()
{
	std::string to_client;
	to_client = std::string("\n")
		  + "##########################\n"
	          + "Do you want to SHOW or SET the topic?\n"
	          + "Type SHOW or SET for the respective option\n";
	std::cout << to_blue(to_client);

	std::string resp;
	getline(std::cin, resp);

	if (resp == "SHOW") {
		to_client = std::string("\n")
			  + "##########################\n"
			  + _user.get_chan() + " Topic:\n"
			  + _channel_topic + "\n\n";
		std::cout << to_blue(to_client);
	} else if (resp == "SET") {
		//@TODO: still not finished with this
		to_client = std::string("\n")
			  + "##########################\n"
			  + "Please type in the channel's new topic:\n";
		std::cout << to_blue(to_client);

		getline(std::cin, resp);

		std::string topic_msg;
		topic_msg = "TOPIC " + _user.get_chan()
			  + " :" + resp + "\r\n";
		try_writing(topic_msg);

		std::string reply = try_reading();
		_channel_topic = parse_topic_msg(reply);

		to_client = std::string("\n")
		    	  + "##########################\n"
		    	  + _user.get_chan() + " Topic:\n"
		          + _channel_topic + "\n";
		std::cout << to_blue(to_client);

	} else {
		std::cout << to_blue("Sorry that wasn't an option\n");
	}
}

// return true if need to quit
bool Client::parse_user_input(std::string msg)
{
	if (msg == "") {
		return false;
	} else if (msg == "EXIT") {
		std::string part_msg = "PART " + _user.get_chan() + "\r\n";
		// ignoring :<part-msg>
		try_writing(part_msg);
		return true;
	} else if (msg == "HELP") {
		std::string to_client;
		to_client = std::string("options...\n")
			  + "TOPIC: SET or SHOW the current chat topic\n"
		          + "EXIT: exit the client\n"
		          + "HELP: print this dialog\n";
		std::cout << to_blue(to_client);
		return false;
	} else if (msg == "TOPIC") {
		handle_topic_request();
		return false;
	} else {
		std::string priv_msg;
		priv_msg = "PRIVMSG " + _user.get_chan()
		         + " :" + msg + "\r\n";
		try_writing(priv_msg);
		return false;
	}
}

void Client::set_sock(boost::asio::io_service& ios)
{
	_sockptr = std::make_unique<tcp::socket>(ios);
}

void Client::run(std::string serv_ip, std::string port)
{
	try {
		std::ios_base::sync_with_stdio(false);
		std::cout << "Starting client...\n";
		int serv_port = std::stoi(port);

		// might need to add support for strings other than localhost
		if (serv_ip == "localhost")
			serv_ip = "127.0.0.1";

		query_and_create();

		boost::asio::io_service io_service;
		tcp::endpoint endpoint(
				boost::asio::ip::address_v4::from_string(serv_ip),
				serv_port);
		//tcp::socket serv_sock(io_service);
		set_sock(io_service);

		//serv_sock.connect(endpoint);
		_sockptr->connect(endpoint);

		pass_user_info_to_server();

		std::string channel = connect_to_channel();
		_user.set_channel(channel);
		_channel_name = channel;

		std::string msg;
		msg = std::string("\n")
		    + "##########################\n"
		    + "Type and press <ENTER> to send a message\n"
		    + "Type EXIT and press <ENTER> to exit the client\n"
		    + "EXIT<ENTER>\n"
		    + "Type HELP and press <ENTER> to find out more\n"
		    + "HELP<ENTER>\n"
		    + "Have fun :)\n\n";
		std::cout << to_blue(msg);

		bool quit = false;
		while (!quit) {
			update();

			for (auto& msg : _sock_msgs)
				parse_session_msg(msg);
			_sock_msgs.clear();

			std::cout << to_cyan(_user.get_nick() + ": ");
			getline(std::cin, msg);

			quit = parse_user_input(msg);
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
}
