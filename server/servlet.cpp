/*
 * servlet.cpp
 *
 * Zeke Reyna
 *
 * @TODO: all need to be wrapped in try / catch?
 */

#include "servlet.h"
#include "../libs/sockio.h"

std::deque<tcp::socket>::iterator Servlet::get_sock_for_user(User user)
{
	try {
		auto it = _socks.begin();
		for (; it != _socks.end(); ++it) {
			if (it->remote_endpoint() == user.get_endpt())
				break;
		}
		return it;
	}
	catch (std::exception& e) {
		std::cout << "ERROR: " << e.what() << std::endl;
		std::cout << "get_sock_for_user" << std::endl;
		throw;
	}
}

std::string Servlet::try_reading(User user)
{
	try {
	tcp::endpoint end = user.get_endpt();
	tcp::socket& sock = *get_sock_for_user(user);
	if (_end_msgs.count(end))
		_end_msgs[end];
	std::deque<std::string>& sock_msgs = _end_msgs[end];
	return try_reading_from_sock(sock, sock_msgs);
	}
	catch (std::exception& e) {
		std::cout << "ERROR: " << e.what() << std::endl;
		std::cout << "try_reading" << std::endl;
		throw;
	}
}

void Servlet::try_writing(User user, std::string msg)
{
	try {
	tcp::socket& sock = *get_sock_for_user(user);
	try_writing_to_sock(sock, msg);
	}
	catch (std::exception& e) {
		std::cout << "ERROR: " << e.what() << std::endl;
		std::cout << "try_writing" << std::endl;
		throw;
	}
}

void Servlet::update_endmsgs()
{
	try {
	for (auto sock_it = _socks.begin(); sock_it != _socks.end(); ++sock_it) {
		tcp::socket& sock = *sock_it;
		tcp::endpoint end = sock.remote_endpoint();
		if (!_end_msgs.count(end))
			_end_msgs[end]; // instantiate because I'm a paranoid boy
		std::deque<std::string>& sock_msgs = _end_msgs[end];
		update_sockmsgs(sock, sock_msgs);
	}
	}
	catch (std::exception& e) {
		std::cout << "ERROR: " << e.what() << std::endl;
		std::cout << "update_endmsgs" << std::endl;
		throw;
	}
}

std::deque<User> Servlet::grab_new()
{
	try {
	// get the new user, and their messages read from socket thus far
	std::unique_lock<std::mutex> lck(gl_lock);
	std::string chan = _channel_name;
	std::deque<User> newusers;
	for (auto tup_it = chan_newusers[chan].begin(); tup_it != chan_newusers[chan].end(); ++tup_it) {
		// copy over user
		User newu = std::get<0>(*tup_it);
		add_user(newu);

		//std::cout << newu.print_() << std::endl;

		// copy over messages
		std::deque<std::string> temp_msgs(std::move(std::get<1>(*tup_it)));
		_end_msgs[newu.get_endpt()]; // initialize
		for (auto msg = temp_msgs.begin(); msg != temp_msgs.end(); ++msg) {
			_end_msgs[newu.get_endpt()].push_back(*msg);
		}

		// for later handling of new users
		newusers.push_back(newu);
	}
	chan_newusers[chan].clear();

	// grab new sockets
	for (auto sock_it = global_socks.begin(); sock_it != global_socks.end(); ) {
		bool match = false;
		for (auto user = _users.begin(); user != _users.end(); ++user) {
			if (sock_it->remote_endpoint() == user->get_endpt()) {
				match = true;
				break;
			}
		}

		if (match) {
			_socks.push_back(std::move(*sock_it));
			sock_it = global_socks.erase(sock_it);
		} else {
			++sock_it;
		}
	}
	return newusers;
	}
	catch (std::exception& e) {
		std::cout << "ERROR: " << e.what() << std::endl;
		std::cout << "grab_new" << std::endl;
		throw;
	}
}

bool Servlet::check_user_in(User user, std::deque<User> deq)
{
	try {
	bool found = false;
	for (auto it = deq.begin(); it != deq.end(); ++it) {
		if (user.get_endpt() == it->get_endpt()) {
			found = true;
			break;
		}
	}
	return found;
	}
	catch (std::exception& e) {
		std::cout << "ERROR: " << e.what() << std::endl;
		std::cout << "check_user_in" << std::endl;
		throw;
	}
}

void Servlet::handle_newusers()
{
	try {
	std::deque<User> newusers(grab_new());
	std::deque<User> handled;
	for (auto it = newusers.begin(); it != newusers.end(); ++it) {
		User newuser(*it); // won't use 'it' after this

		handled.push_back(newuser);

		std::string remIP = newuser.get_endpt().address().to_string();
		std::string msg;
		msg  = newuser.get_nick() + "!" + newuser.get_user() + "@" + remIP;
		msg += " JOIN " + newuser.get_chan() + "\r\n";

		std::string usernames = "";
		for (auto it2 = _users.begin(); it2 != _users.end(); ++it2) {
			User user(*it2);
			if (check_user_in(user, newusers) && !check_user_in(user, handled))
				continue;
			try_writing(user, msg);

			usernames += "@" + user.get_nick() + " ";
		}
		std::string locIP;
		tcp::socket& sock = *get_sock_for_user(newuser);

		locIP = sock.local_endpoint().address().to_string();
		msg  = locIP + " " + RPL_TOPIC + " ";
		msg += newuser.get_nick() + " " + newuser.get_chan() + " ";
		msg += ":" + _topic + "\r\n";
		try_writing(newuser, msg);

		msg  = locIP + " " + RPL_NAMREPLY + " ";
		msg += newuser.get_nick() + " " + newuser.get_chan() + " ";
		msg += ":" + usernames + "\r\n";
		try_writing(newuser, msg);

		msg  = locIP + " " + RPL_ENDOFNAMES + " ";
		msg += newuser.get_nick() + " " + newuser.get_chan() + " ";
		msg += ":End of NAMES list\r\n";
		try_writing(newuser, msg);
	}
	}
	catch (std::exception& e) {
		std::cout << "ERROR: " << e.what() << std::endl;
		std::cout << "handle_newusers" << std::endl;
		throw;
	}
}

bool Servlet::check_newusers()
{
	try {
	std::unique_lock<std::mutex> lck(gl_lock);
	if (chan_newusers.count(_channel_name) && !chan_newusers[_channel_name].empty())
		return true;
	else
		return false;
	}
	catch (std::exception& e) {
		std::cout << "ERROR: " << e.what() << std::endl;
		std::cout << "check_newusers" << std::endl;
		throw;
	}
}

bool Servlet::check_endmsgs()
{
	try {
	for (auto it = _end_msgs.begin(); it != _end_msgs.end(); ++it) {
		if (!it->second.empty())
			return true;
	}
	return false;
	}
	catch (std::exception& e) {
		std::cout << "ERROR: " << e.what() << std::endl;
		std::cout << "check_endmsgs" << std::endl;
		throw;
	}
}

void Servlet::handle_msg(std::string msg, tcp::endpoint end)
{
	try {
	User client;
	for (auto it = _users.begin(); it != _users.end(); ++it) {
		if (it->get_endpt() == end) {
			client = User(*it);
			break;
		}
	}

	if (msg.substr(0, 7) == "PRIVMSG") {
		std::cout << "in privmsg" << std::endl;
		std::cout << "MSG: " << msg << std::endl;
		// format read:
		// PRIVMSG <channel> :<msg>
		// format to write:
		// :<nick>!<user>@<user-ip> PRIVMSG <channel> :<msg>

		std::string clntIP = end.address().to_string();
		std::string reply;
		reply  = ":" + client.get_nick() + "!" + client.get_user() + "@";
		reply += clntIP + " " + msg + "\r\n";
		for (auto it = _users.begin(); it != _users.end(); ++it) {
			if (it->get_endpt() == end)
				continue;
			// broadcast PRIVMSG to members besides one who sent it
			try_writing(*it, reply);
		}

	} else if (msg.substr(0, 4) == "PART") {
		std::cout << "in part" << std::endl;
		std::cout << "MSG: " << msg << std::endl;
		// format read:
		// PART <channel>
		// format to write:
		// :<nick>!<user>@<user-ip> PART <channel>

		std::string clntIP = end.address().to_string();
		std::string reply;
		reply  = ":" + client.get_nick() + "!" + client.get_user() + "@";
		reply += clntIP + " " + msg + "\r\n";
		for (auto it = _users.begin(); it != _users.end(); ++it) {
			if (it->get_endpt() == end)
				continue;
			try_writing(*it, reply);
		}

		// remove user from users deque
		// del entry for end_msgs and end_socks
		for (auto it = _users.begin(); it != _users.end(); ++it) {
			if (it->get_endpt() == end) {
				_users.erase(it);
				break;
			}
		}

		// get rid of deque associated with socket
		_end_msgs.erase(end);

		// get rid of socket
		for (auto it = _socks.begin(); it != _socks.end(); ++it) {
			boost::system::error_code ec;
			tcp::endpoint this_end = it->remote_endpoint(ec);
			if (ec) {
				// error thrown, so socket was closed and this is one
				// to get rid of.
				_socks.erase(it);
				break;
			} if (this_end == end) {
				_socks.erase(it); // calling erase like this should
				// implicitly close socket
				break;
			}
		}

	} else {
		std::cout << "Unrecognized Message:" << std::endl;
		std::cout << "Msg begin: ";
		std::cout << msg << std::endl;
		std::cout << "Msg end";
		throw std::invalid_argument("Unrecognized message format");
	}
	}
	catch (std::exception& e) {
		std::cout << "ERROR: " << e.what() << std::endl;
		std::cout << "handle_msg" << std::endl;
		throw;
	}
}

void Servlet::handle_endmsgs()
{
	try {
	for (auto em_it = _end_msgs.begin(); em_it != _end_msgs.end(); ++em_it) {
		tcp::endpoint end(em_it->first);
		std::deque<std::string>& msgs = em_it->second;

		for (auto msg = msgs.begin(); msg != msgs.end(); ++msg) {
			handle_msg(*msg, end);
		}
		msgs.clear(); // because reference should clear the actual
	}
	}
	catch (std::exception& e) {
		std::cout << "ERROR: " << e.what() << std::endl;
		std::cout << "handle_endmsgs" << std::endl;
		throw;
	}
}

void run(std::string channel)
{
	try {
		Servlet servlet(channel);
		while (!killself) {
			bool check = servlet.check_newusers();
			if (check)
				servlet.handle_newusers();

			servlet.update_endmsgs();

			// Handle end_msgs
			servlet.handle_endmsgs();
		}
		// clean up, don't need to close sockets since map and deque should do
		// implicitly by destructor call.
		std::cout << "Thread exiting" << std::endl;
	} catch (const std::exception& e) {
		std::cout << "There was an error:" << std::endl;
		std::cout << e.what() << std::endl;
	} catch (...) {
		// @TODO: check if child threads can catch keyboard exceptions etc.
		// since we want the parent thread to handle that stuff.
		// Might need a signal handler?
		// @TODO: make another catch for catcheable exceptions.
		std::cout << "There was an error" << std::endl;
	}
}
