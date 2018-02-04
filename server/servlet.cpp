/*
 * servlet.cpp
 *
 * Zeke Reyna
 */

#include "servlet.h"
#include "../libs/sockio.h"

std::deque<tcp::socket>::iterator Servlet::get_sock_for_user(User user)
{
	auto it = _socks.begin();
	for (; it != _socks.end(); ++it) {
		if (it->remote_endpoint() == user.get_endpt())
			break;
	}
	return it;
}

std::string Servlet::try_reading(User user)
{
	tcp::endpoint end = user.get_endpt();
	tcp::socket& sock = *get_sock_for_user(user);
	if (_end_msgs.count(end))
		_end_msgs[end];
	std::deque<std::string>& sock_msgs = _end_msgs[end];
	return try_reading_from_sock(sock, sock_msgs);
}

void Servlet::try_writing(User user, std::string msg)
{
	tcp::socket& sock = *get_sock_for_user(user);
	try_writing_to_sock(sock, msg);
}

void Servlet::update_endmsgs()
{
	for (auto& sock : _socks) {
		tcp::endpoint end = sock.remote_endpoint();
		if (!_end_msgs.count(end))
			_end_msgs[end];
		std::deque<std::string>& sock_msgs = _end_msgs[end];
		update_sockmsgs(sock, sock_msgs);
	}
}

std::deque<User> Servlet::grab_new()
{
	// make pointers with local references
	std::mutex& gl_lock = *_gl_lock_ptr;
	auto& chan_newusers = *_chan_newusers_ptr;
	auto& global_socks = *_global_socks_ptr;

	// get the new user, and their messages read from socket thus far
	std::unique_lock<std::mutex> lck(gl_lock);
	std::string chan = _channel_name;
	std::deque<User> newusers;

	for (auto& tup : chan_newusers[chan]) {
		// copy over user
		User newu = std::get<0>(tup);
		_users.push_back(newu);

		//std::cout << newu.print_() << std::endl;

		// copy over messages
		std::deque<std::string> temp_msgs(std::move(std::get<1>(tup)));
		_end_msgs[newu.get_endpt()]; // initialize

		for (auto& msg : temp_msgs)
			_end_msgs[newu.get_endpt()].push_back(msg);

		// for later handling of new users
		newusers.push_back(newu);
	}
	chan_newusers[chan].clear();

	// grab new sockets
	for (auto sock_it = global_socks.begin(); sock_it != global_socks.end(); ) {
		bool match = false;

		for (auto& user : _users) {
			if (user.get_endpt() == sock_it->remote_endpoint()) {
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

bool Servlet::check_user_in(User& user, std::deque<User>& deq)
{
	bool found = false;
	for (auto& u : deq) {
		if (u.get_endpt() == user.get_endpt()) {
			found = true;
			break;
		}
	}
	return found;
}

void Servlet::handle_newusers()
{
	std::deque<User> newusers(grab_new());
	std::deque<User> handled;
	for (auto& newuser : newusers) {

		handled.push_back(newuser);

		std::string remIP = newuser.get_endpt().address().to_string();
		std::string msg;
		msg = newuser.get_nick() + "!" + newuser.get_user() + "@" + remIP
		    + " JOIN " + newuser.get_chan() + "\r\n";

		std::string usernames = "";
		for (auto& user : _users) {
			if (check_user_in(user, newusers) && !check_user_in(user, handled))
				continue;
			try_writing(user, msg);

			usernames += "@" + user.get_nick() + " ";
		}
		std::string locIP;
		tcp::socket& sock = *get_sock_for_user(newuser);

		locIP = sock.local_endpoint().address().to_string();
		msg = locIP + " " + RPL_TOPIC + " "
		    + newuser.get_nick() + " " + newuser.get_chan() + " "
		    + ":" + _topic + "\r\n";
		try_writing(newuser, msg);

		msg = locIP + " " + RPL_NAMREPLY + " "
		    + newuser.get_nick() + " " + newuser.get_chan() + " "
		    + ":" + usernames + "\r\n";
		try_writing(newuser, msg);

		msg = locIP + " " + RPL_ENDOFNAMES + " "
		    + newuser.get_nick() + " " + newuser.get_chan() + " "
		    + ":End of NAMES list\r\n";
		try_writing(newuser, msg);
	}
}

bool Servlet::check_newusers()
{
	auto& gl_lock = *_gl_lock_ptr;
	auto& chan_newusers = *_chan_newusers_ptr;

	std::unique_lock<std::mutex> lck(gl_lock);
	if (chan_newusers.count(_channel_name) && !chan_newusers[_channel_name].empty())
		return true;
	else
		return false;
}

bool Servlet::check_endmsgs()
{
	for (auto& em : _end_msgs) {
		if (!em.second.empty())
			return true;
	}
	return false;
}

void Servlet::handle_msg(std::string msg, tcp::endpoint end)
{
	User client;
	for (auto& u : _users) {
		if (u.get_endpt() == end) {
			client = u;
			break;
		}
	}

	if (msg.substr(0, 7) == "PRIVMSG") {
		std::cout << "in privmsg\n";
		std::cout << "MSG: \n";
		// format read:
		// PRIVMSG <channel> :<msg>
		// format to write:
		// :<nick>!<user>@<user-ip> PRIVMSG <channel> :<msg>

		std::string clntIP = end.address().to_string();
		std::string reply;
		reply = ":" + client.get_nick() + "!" + client.get_user() + "@"
		      + clntIP + " " + msg + "\r\n";
		for (auto& u : _users) {
			if (u.get_endpt() == end)
				continue;
			// broadcast PRIVMSG to members besides one who sent it
			try_writing(u, reply);
		}

	} else if (msg.substr(0, 4) == "PART") {
		std::cout << "in part\n";
		std::cout << "MSG: \n";
		// format read:
		// PART <channel>
		// format to write:
		// :<nick>!<user>@<user-ip> PART <channel>

		std::string clntIP = end.address().to_string();
		std::string reply;
		reply = ":" + client.get_nick() + "!" + client.get_user() + "@"
		      + clntIP + " " + msg + "\r\n";
		for (auto& u : _users) {
			if (u.get_endpt() == end)
				continue;
			try_writing(u, reply);
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
		std::cout << "Unrecognized Message:\n";
		std::cout << "Msg begin: ";
		std::cout << msg << "\n";
		std::cout << "Msg end";
		throw std::invalid_argument("Unrecognized message format");
	}
}

void Servlet::handle_endmsgs()
{
	for (auto& em : _end_msgs) {
		tcp::endpoint end = em.first;
		std::deque<std::string>& msgs = em.second;

		for (auto& msg : msgs)
			handle_msg(msg, end);

		if (_end_msgs.count(end) && !msgs.empty())
			msgs.clear(); // because reference should clear the actual
	}
}

void run(std::string channel,
		std::map<std::string, std::deque<std::tuple<User, std::deque<std::string>>
		>> *chan_newusers_ptr,
		std::deque<tcp::socket> *global_socks_ptr,
		std::mutex *gl_lock_ptr)
{
	try {
		Servlet servlet(channel, chan_newusers_ptr, global_socks_ptr, gl_lock_ptr);
		while (!killself) {
			bool check = servlet.check_newusers();
			if (check)
				servlet.handle_newusers();

			servlet.update_endmsgs();

			// Handle end_msgs
			check = servlet.check_endmsgs();
			if (check)
				servlet.handle_endmsgs();
		}
		// clean up, don't need to close sockets since map and deque should do
		// implicitly by destructor call.
		std::cout << "Thread exiting\n";
	} catch (const std::exception& e) {
		std::cout << "There was an error:\n";
		std::cout << e.what() << std::endl;
	}
}
