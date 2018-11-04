/*
 * servlet.cpp
 *
 * Zeke Reyna
 */

#include "Servlet.h"
#include <utility>
#include <ircConstants.h>

SocketList::iterator Servlet::get_sock_for_user(User user)
{
	return std::find_if(_socks.begin(), _socks.end(),
			[&user](const tcp::socket& sock)
			{ return user.get_endpt() == sock.remote_endpoint(); });
}

void Servlet::remove_trace_of(User user)
{
	// remove user from users deque
	const tcp::endpoint END_POINT = user.get_endpt();
	_users.erase(std::find_if(_users.begin(), _users.end(),
		[&END_POINT] (const auto& u) { return u.get_endpt() == END_POINT; }));

	// get rid of deque associated with socket
	_end_msgs.erase(END_POINT);

	// get rid of socket
	using Iter = SocketList::iterator;
	for (Iter it = _socks.begin(); it != _socks.end(); ++it) {
		asio::error_code ec;
		const tcp::endpoint THIS_END = it->remote_endpoint(ec);
		if (ec) {
			// error thrown, so socket was closed and this is one
			// to get rid of.
			_socks.erase(it);
			break;
		} if (THIS_END == END_POINT) {
			_socks.erase(it); // calling erase like this should
			// implicitly close socket
			break;
		}
	}

}

std::string Servlet::try_reading(User user)
{
	const tcp::endpoint END_POINT = user.get_endpt();
	tcp::socket& sock = *get_sock_for_user(user);
	MsgList& sock_msgs = _end_msgs[END_POINT];
	return sockio::try_reading_from_sock(sock, sock_msgs);
}

void Servlet::try_writing(User user, std::string msg)
{
	tcp::socket& sock = *get_sock_for_user(user);
	const asio::error_code EC = sockio::try_writing_to_sock(sock, std::move(msg));
	if (EC) {
		// assume broken sock or something... remove user
		remove_trace_of(user);
	}
}

void Servlet::update_endmsgs()
{
	for (tcp::socket& sock : _socks) {
		const tcp::endpoint END_POINT = sock.remote_endpoint();
		MsgList& sock_msgs = _end_msgs[END_POINT];
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
	const std::string CHANNEL = _channel_name;
	std::deque<User> newusers;

	for (auto& tup : chan_newusers[CHANNEL]) {
		// copy over user and msgs
		const auto& [NEW_USER, TEMP_MSGS] = tup;

		_users.push_back(NEW_USER);
		_end_msgs[NEW_USER.get_endpt()]; // initialize
		for (const std::string& MSG : TEMP_MSGS)
			_end_msgs[NEW_USER.get_endpt()].push_back(MSG);

		// for later handling of new users
		newusers.push_back(NEW_USER);
	}
	chan_newusers[CHANNEL].clear();

	// grab new sockets
	using Iter = SocketList::iterator;
	for (Iter sock_it = global_socks.begin(); sock_it != global_socks.end(); ) {
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
	for (const User& NEW_USER: newusers) {

		handled.push_back(NEW_USER);

		const std::string REM_IP = NEW_USER.get_endpt().address().to_string();
		std::string msg;
		msg = NEW_USER.get_nick() + "!" + NEW_USER.get_whoami() + "@" + REM_IP
		    + " JOIN " + NEW_USER.get_chan() + "\r\n";

		std::string usernames;
		for (auto& user : _users) {
			if (check_user_in(user, newusers) && !check_user_in(user, handled))
				continue;
			try_writing(user, msg);

			usernames += "@" + user.get_nick() + " ";
		}
		tcp::socket& sock = *get_sock_for_user(NEW_USER);

		const std::string LOC_IP = sock.local_endpoint().address().to_string();
		msg = LOC_IP + " " + RPL_TOPIC + " "
		    + NEW_USER.get_nick() + " " + NEW_USER.get_chan() + " "
		    + ":" + _channel_topic + "\r\n";
		try_writing(NEW_USER, msg);

		msg = LOC_IP + " " + RPL_NAMREPLY + " "
		    + NEW_USER.get_nick() + " " + NEW_USER.get_chan() + " "
		    + ":" + usernames + "\r\n";
		try_writing(NEW_USER, msg);

		msg = LOC_IP + " " + RPL_ENDOFNAMES + " "
		    + NEW_USER.get_nick() + " " + NEW_USER.get_chan() + " "
		    + ":End of NAMES list\r\n";
		try_writing(NEW_USER, msg);
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
	const std::string CLIENT_IP = end.address().to_string();
	std::string reply;
	reply = ":" + client.get_nick() + "!" + client.get_whoami() + "@"
	      + CLIENT_IP + " " + msg + "\r\n";
	for (auto& u : _users) {
		if (u.get_endpt() == end)
			continue;
		// broadcast PRIVMSG to members besides one who sent it
		try_writing(u, reply);
	}

	// then, log msg into database.
	// get substring, so that only the _actual_ message content is sent to database.
	const std::size_t POS = msg.find(":");
	chitter::insertMsg(_channel_name, client.get_nick(), msg.substr(POS + 1),
			_server_name, _connection);
}

void Servlet::handle_part(std::string msg, User client)
{
	// format read:
	// PART <channel>
	// format to write:
	// :<nick>!<user>@<user-ip> PART <channel>

	const tcp::endpoint end = client.get_endpt();
	const std::string CLIENT_IP = end.address().to_string();
	std::string reply;
	reply = ":" + client.get_nick() + "!" + client.get_whoami() + "@"
	      + CLIENT_IP + " " + msg + "\r\n";
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
	MsgList parts = sockio::split(std::move(msg), split_here);
	_channel_topic = *--parts.end();

	// need to update topic in database
	chitter::updateChannelTopic(_channel_name, _channel_topic,
			_server_name, _connection);

	const tcp::socket& SOCK = _socks.front();
	//const std::string LOC_IP = SOCK.local_endpoint().address().to_string();

	std::string reply;
	reply = client.get_nick() + "!" + client.get_whoami() + "@"
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
    using MapPair = std::pair<const tcp::endpoint, MsgList>;
	for (MapPair& em : _end_msgs) {
		const tcp::endpoint& END_POINT = em.first;
		MsgList& msgList = em.second;

		for (const std::string& MSG: msgList)
			handle_msg(MSG, END_POINT);

		if (_end_msgs.count(END_POINT) && !msgList.empty())
			msgList.clear(); // because reference should clear the actual
	}
}

void thread_run(const std::string server, const std::string channel, const std::string topic,
		Chan_newusers_ptr chan_newusers_ptr,
	 SocketList *global_socks_ptr, std::mutex *gl_lock_ptr,
	 std::shared_ptr<std::atomic<bool>> notify)
{
	try {
		Servlet servlet(server, std::move(channel), topic, chan_newusers_ptr, global_socks_ptr, gl_lock_ptr);
		while (!selfdestruct) {
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
