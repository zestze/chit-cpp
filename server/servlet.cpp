/*
 * servlet.cpp
 *
 * Zeke Reyna
 */

#include "servlet.h"

std::deque<tcp::socket>::iterator Servlet::get_sock_for_user(User user)
{
	return std::find_if(_socks.begin(), _socks.end(),
			[&user](const auto& sock)
			{ return user.get_endpt() == sock.remote_endpoint(); });
}

void Servlet::remove_trace_of(User user)
{
	// remove user from users deque
	tcp::endpoint end = user.get_endpt();
	_users.erase(std::find_if(_users.begin(), _users.end(),
		[&end] (const auto& u) { return u.get_endpt() == end; }));

	// get rid of deque associated with socket
	_end_msgs.erase(end);

	// get rid of socket
	//_socks.erase(get_sock_for_user(user));
	for (auto it = _socks.begin(); it != _socks.end(); ++it) {
		asio::error_code ec;
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

}

std::string Servlet::try_reading(User user)
{
	tcp::endpoint end = user.get_endpt();
	tcp::socket& sock = *get_sock_for_user(user);
	if (_end_msgs.count(end))
		_end_msgs[end];
	std::deque<std::string>& sock_msgs = _end_msgs[end];
	return sockio::try_reading_from_sock(sock, sock_msgs);
}

void Servlet::try_writing(User user, std::string msg)
{
	tcp::socket& sock = *get_sock_for_user(user);
	asio::error_code ec = sockio::try_writing_to_sock(sock, msg);
	if (ec) {
		// assume broken sock or something... remove user
		remove_trace_of(user);
	}
}

void Servlet::update_endmsgs()
{
	for (auto& sock : _socks) {
		tcp::endpoint end = sock.remote_endpoint();
		if (!_end_msgs.count(end))
			_end_msgs[end];
		std::deque<std::string>& sock_msgs = _end_msgs[end];
		sockio::update_sockmsgs(sock, sock_msgs);
	}
}

std::deque<User> Servlet::grab_new()
{
	// make pointers with local references
	std::mutex& gl_lock = *_gl_lock_ptr;
	auto& chan_newusers = *_chan_newusers_ptr;
	auto& global_socks = *_global_socks_ptr;

	// get the new user, and their messages read from socket thus far
	std::lock_guard<std::mutex> lck(gl_lock);
	std::string chan = _channel_name;
	std::deque<User> newusers;

	for (auto& tup : chan_newusers[chan]) {
		// copy over user and msgs
		auto& [newu, temp_msgs] = tup;

		_users.push_back(std::move(newu));
		_end_msgs[newu.get_endpt()]; // initialize
		for (auto msg : temp_msgs)
			_end_msgs[newu.get_endpt()].push_back(msg);

		// for later handling of new users
		newusers.push_back(newu);
	}
	chan_newusers[chan].clear();

	// grab new sockets
	for (auto sock_it = global_socks.begin(); sock_it != global_socks.end(); ) {
		bool match = std::any_of(_users.begin(), _users.end(),
				[sock_it] (const auto& user)
				{ return user.get_endpt() ==
				sock_it->remote_endpoint(); });

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
	return std::any_of(deq.begin(), deq.end(), [&user] (const auto& u)
			{ return user.get_endpt() == u.get_endpt(); });
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
		    + ":" + _channel_topic + "\r\n";
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

bool Servlet::check_newusers(std::shared_ptr<std::atomic<bool>>& notify)
{
	if (*notify)
		return true;
	else
		return false;
}

bool Servlet::check_endmsgs()
{
	return std::any_of(_end_msgs.begin(), _end_msgs.end(),
			[] (const auto& em) { return !em.second.empty(); });
}

void Servlet::handle_priv(std::string msg, User client)
{
	// format read:
	// PRIVMSG <channel> :<msg>
	// format to write:
	// :<nick>!<user>@<user-ip> PRIVMSG <channel> :<msg>

	const tcp::endpoint end = client.get_endpt();
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
}

void Servlet::handle_part(std::string msg, User client)
{
	// format read:
	// PART <channel>
	// format to write:
	// :<nick>!<user>@<user-ip> PART <channel>

	const tcp::endpoint end = client.get_endpt();
	std::string clntIP = end.address().to_string();
	std::string reply;
	reply = ":" + client.get_nick() + "!" + client.get_user() + "@"
	      + clntIP + " " + msg + "\r\n";
	for (auto& u : _users) {
		if (u.get_endpt() == end)
			continue;
		try_writing(u, reply);
	}

	remove_trace_of(client);
}

void Servlet::handle_topic(std::string msg, User client)
{
	// format read:
	// TOPIC <channel> :<new-topic>
	// format to write:
	// nick!<user>@<user-ip> TOPIC <channel> :<new-topic>
	// Going to send a RPL_TOPIC to person sending the update,
	// and to the others, will be similar to a PRIVMSG

	std::string split_here = "TOPIC " + _channel_name + " :";
	std::deque<std::string> parts = sockio::split(msg, split_here);
	_channel_topic = *--parts.end();

	const tcp::socket& sock = _socks.front();
	const std::string locIP = sock.local_endpoint().address().to_string();

	std::string reply;
	reply = client.get_nick() + "!" + client.get_user() + "@"
	      + client.get_endpt().address().to_string()
	      + " TOPIC " + _channel_name + " :"
	      + _channel_topic + "\r\n";

	for (auto& u : _users)
		try_writing(u, reply);
}

void Servlet::handle_msg(std::string msg, tcp::endpoint end)
{
	User client = *std::find_if(_users.begin(), _users.end(),
			[&end] (const auto& u) { return u.get_endpt() == end; });

	if (msg.substr(0, 7) == "PRIVMSG") {
		handle_priv(msg, client);

	} else if (msg.substr(0, 4) == "PART") {
		handle_part(msg, client);

	} else if (msg.substr(0, 5) == "TOPIC") {
		handle_topic(msg, client);

	} else {
		std::cerr << "Unrecognized Message:\n"
			  << "Msg begin: " << msg << "\n" << "Msg end";
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

void thread_run(std::string channel, Chan_newusers_ptr chan_newusers_ptr,
	 std::deque<tcp::socket> *global_socks_ptr, std::mutex *gl_lock_ptr,
	 std::shared_ptr<std::atomic<bool>> notify)
{
	try {
		Servlet servlet(channel, chan_newusers_ptr, global_socks_ptr, gl_lock_ptr);
		while (!killself) {
			bool check = servlet.check_newusers(notify);
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
		std::cerr << "There was an error:\n";
		std::cerr << e.what() << std::endl;
	}
}
