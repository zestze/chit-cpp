/*
 * servlet.cpp
 *
 * Zeke Reyna
 *
 * @TODO: all need to be wrapped in try / catch?
 *
 * @TODO: revamp split_ method, give better name, or use boost regex
 */

#include "servlet.h"
#include "../libs/sockio.h"

#define 	BUFF_SIZE 		1024

// returns iterator
/*
std::deque<tcp::socket>::iterator get_sock_for_user(Servlet& srvlt, User usr)
{
	auto it = srvlt.socks.begin();
	for (; it != srvlt.socks.end(); ++it) {
		if (it->remote_endpoint() == usr.get_endpt())
			break;
	}
	return it;
}
*/

std::deque<tcp::socket>::iterator Servlet::get_sock_for_user(User user)
{
	auto it = _socks.begin();
	for (; it != _socks.end(); ++it) {
		if (it->remote_endpoint() == user.get_endpt())
			break;
	}
	return it;
}

/*
std::string try_reading(Servlet& servlet, User user)
{
	tcp::endpoint end = user.get_endpt();
	tcp::socket& sock = *get_sock_for_user(servlet, user);
	if (!servlet.end_msgs.count(end))
		servlet.end_msgs[end]; // instantiate because i'm a paranoid boy
	std::deque<std::string>& sock_msgs = servlet.end_msgs[end];
	return try_reading_from_sock(sock, sock_msgs);
}
*/
std::string Servlet::try_reading(User user)
{
	tcp::endpoint end = user.get_endpt();
	tcp::socket& sock = *get_sock_for_user(user);
	if (_end_msgs.count(end))
		_end_msgs[end];
	std::deque<std::string>& sock_msgs = _end_msgs[end];
	return try_reading_from_sock(sock, sock_msgs);
}

/*
void try_writing(Servlet& srvlt, User usr, std::string msg)
{
	tcp::socket& sock = *get_sock_for_user(srvlt, usr);
	try_writing_to_sock(sock, msg);
}
*/

void Servlet::try_writing(User user, std::string msg)
{
	tcp::socket& sock = *get_sock_for_user(user);
	try_writing_to_sock(sock, msg);
}

/*
void update_end_msgs(Servlet& servlet)
{
	for (auto sock_it = servlet.socks.begin(); sock_it != servlet.socks.end(); ++sock_it) {
		tcp::socket& sock = *sock_it;
		tcp::endpoint end = sock.remote_endpoint();
		if (!servlet.end_msgs.count(end))
			servlet.end_msgs[end]; // instantiate because I'm a paranoid boy
		std::deque<std::string>& sock_msgs = servlet.end_msgs[end];
		update_sockmsgs(sock, sock_msgs);
	}
}
*/
void Servlet::update_endmsgs()
{
	for (auto sock_it = _socks.begin(); sock_it != _socks.end(); ++sock_it) {
		tcp::socket& sock = *sock_it;
		tcp::endpoint end = sock.remote_endpoint();
		if (!_end_msgs.count(end))
			_end_msgs[end]; // instantiate because I'm a paranoid boy
		std::deque<std::string>& sock_msgs = _end_msgs[end];
		update_sockmsgs(sock, sock_msgs);
	}
}

/*
std::deque<User> grab_new(Servlet& servlet)
{
	// formerly grab_newusers,
	// get the new user, and their messages read from socket thus far
	std::unique_lock<std::mutex> lck(gl_lock);
	std::string chan = servlet.get_chan();
	std::deque<User> newusers;
	for (auto tup_it = chan_newusers[chan].begin(); tup_it != chan_newusers[chan].end(); ++tup_it) {
		// copy over user
		User new_u = std::get<0>(*tup_it);
		servlet.add_user(new_u);

		std::cout << new_u.print_() << std::endl;

		// copy over messages
		//servlet.end_msgs[new_u.get_endpt()] = std::get<1>(*tup_it);
		std::deque<std::string> temp_msgs(std::get<1>(*tup_it));
		servlet.end_msgs[new_u.get_endpt()]; // initialize
		for (auto msg = temp_msgs.begin(); msg != temp_msgs.end(); ++msg) {
			servlet.end_msgs[new_u.get_endpt()].push_back(*msg);
		}

		// for later handling of new users
		newusers.push_back(new_u);
	}
	chan_newusers[chan].clear();


	// formerly get_newsockets
	for (auto sock_it = global_socks.begin(); sock_it != global_socks.end(); ) {
		std::cout << sock_it->remote_endpoint().address().to_string() << std::endl;
		bool match = false;
		for (auto usr = servlet.users.begin(); usr != servlet.users.end(); ++usr) {
			if (sock_it->remote_endpoint() == usr->get_endpt()) {
				match = true;
				break;
			}
		}

		if (match) {
			servlet.socks.push_back(std::move(*sock_it));
			sock_it = global_socks.erase(sock_it);
		} else {
			++sock_it;
		}
	}
	return newusers;
}
*/
std::deque<User> Servlet::grab_new()
{
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
		std::deque<std::string> temp_msgs(std::get<1>(*tup_it));
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

/*
bool check_user_in(User user, std::deque<User> list_)
{
	bool found = false;
	for (auto it = list_.begin(); it != list_.end(); ++it) {
		if (user.get_endpt() == it->get_endpt()) {
			found = true;
			break;
		}
	}
	return found;
}
*/
bool Servlet::check_user_in(User user, std::deque<User> deq)
{
	bool found = false;
	for (auto it = deq.begin(); it != deq.end(); ++it) {
		if (user.get_endpt() == it->get_endpt()) {
			found = true;
			break;
		}
	}
	return found;
}

/*
void handle_newusers(Servlet& servlet)
{
	std::deque<User> newusers(grab_new(servlet));
	std::deque<User> handled;
	for (auto it = newusers.begin(); it != newusers.end(); ++it) {
		User new_user(*it); // won't use it after this

		// immediately add user to handled, as a hack to fix the check_user_in
		// for the currently-being-handled newuser case
		handled.push_back(new_user);
		std::cout << new_user.print_() << std::endl;

		std::string rem_IP = new_user.get_endpt().address().to_string();
		std::string msg;
		msg  = new_user.get_nick() + "!" + new_user.get_user() + "@" + rem_IP;
		msg += " JOIN " + new_user.get_chan() + "\r\n";

		std::cout << "jumping into handle_newusers middle for\n";
		std::string user_names = "";
		for (auto it2 = servlet.users.begin(); it2 != servlet.users.end(); ++it2) {
			User serv_user(*it2);
			std::cout << "in handle_newusers end for\n";
			// if user is in newusers, but not in handled, it shouldn't
			// be included in curr list of users, and shouldn't be messsaged
			// about this new person.
			if (check_user_in(serv_user, newusers) && !check_user_in(serv_user, handled))
				continue;
			std::cout << "didn't go through continue\n";
			try_writing(servlet, serv_user, msg);

			user_names += "@" + serv_user.get_nick() + " ";
		}
		std::string loc_IP;
		std::deque<tcp::socket>::iterator temp = get_sock_for_user(servlet, new_user);
		tcp::socket& sock = *temp;

		std::cout << "error may be here?\n";
		loc_IP = sock.local_endpoint().address().to_string(); // @TODO: error is here.
		std::cout << "error may be here?\n";
		msg  = loc_IP + " " + RPL_TOPIC + " ";
		msg += new_user.get_nick() + " " + new_user.get_chan() + " ";
		msg += ":" + servlet.get_topic() + "\r\n";
		try_writing(servlet, new_user, msg);

		msg  = loc_IP + " " + RPL_NAMREPLY + " ";
		msg += new_user.get_nick() + " " + new_user.get_chan() + " ";
		msg += ":" + user_names + "\r\n";
		try_writing(servlet, new_user, msg);

		msg  = loc_IP + " " + RPL_ENDOFNAMES + " ";
		msg += new_user.get_nick() + " " + new_user.get_chan() + " ";
		msg += ":End of NAMES list\r\n";
		try_writing(servlet, new_user, msg);
	}
	std::cout << "done with loop\n";

	// now that info is grabbed, need to handle it.
}
*/
void Servlet::handle_newusers()
{
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

bool Servlet::check_newusers()
{
	std::unique_lock<std::mutex> lck(gl_lock);
	if (chan_newusers.count(_channel_name) && !chan_newusers[_channel_name].empty())
		return true;
	else
		return false;
}

/*
// return TRUE if needs handling
bool check_newusers(std::string chan)
{
	std::unique_lock<std::mutex> lck(gl_lock);
	if (chan_newusers.count(chan) && !chan_newusers[chan].empty())
		return true;
	else
		return false;
}
*/

bool Servlet::check_endmsgs()
{
	for (auto it = _end_msgs.begin(); it != _end_msgs.end(); ++it) {
		if (!it->second.empty())
			return true;
	}
	return false;
}

/*
// return TRUE if needs handling
bool check_end_msgs(Servlet& servlet)
{
	for (auto it = servlet.end_msgs.begin(); it != servlet.end_msgs.end(); ++it) {
		if (!it->second.empty())
			return true;
	}
	return false;
}
*/

void Servlet::handle_msg(std::string msg, tcp::endpoint end)
{
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

/*
void handle_msg(std::string msg, Servlet& servlet, tcp::endpoint end)
{
	User client;
	for (auto it = servlet.users.begin(); it != servlet.users.end(); ++it) {
		if (it->get_endpt() == end) {
			client = *it;
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

		std::string clnt_IP = end.address().to_string();
		std::string reply;
		reply  = ":" + client.get_nick() + "!" + client.get_user() + "@";
		reply += clnt_IP + " " + msg + "\r\n";
		for (auto it = servlet.users.begin(); it != servlet.users.end(); ++it) {
			if (it->get_endpt() == end)
				continue;
			// broadcast PRIVMSG to members besides one who sent it
			try_writing(servlet, *it, reply);
		}
	} else if (msg.substr(0, 4) == "PART") {
		std::cout << "in part" << std::endl;
		std::cout << "MSG: " << msg << std::endl;
		// format read:
		// PART <channel>
		// format to write:
		// :<nick>!<user>@<user-ip> PART <channel>

		std::string clnt_IP = end.address().to_string();
		std::string reply;
		reply  = ":" + client.get_nick() + "!" + client.get_user() + "@";
		reply += clnt_IP + " " + msg + "\r\n";
		for (auto it = servlet.users.begin(); it != servlet.users.end(); ++it) {
			if (it->get_endpt() == end)
				continue;
			try_writing(servlet, *it, reply);
		}

		// remove user from users deque
		// del entry for end_msgs and end_socks
		for (auto it = servlet.users.begin(); it != servlet.users.end(); ++it) {
			if (it->get_endpt() == end) {
				servlet.users.erase(it);
				break;
			}
		}

		// get rid of deque associated with socket
		servlet.end_msgs.erase(end);

		// get rid of socket
		for (auto it = servlet.socks.begin(); it != servlet.socks.end(); ++it) {
			boost::system::error_code ec;
			tcp::endpoint this_end = it->remote_endpoint(ec);
			if (ec) {
				// error thrown, so socket was closed and this is one
				// to get rid of.
				servlet.socks.erase(it);
				break;
			} if (this_end == end) {
				servlet.socks.erase(it); // calling erase like this should
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
*/

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
			for (auto em_it = servlet._end_msgs.begin(); em_it != servlet._end_msgs.end();
					++em_it) {
				tcp::endpoint end(em_it->first);
				std::deque<std::string> msgs(em_it->second);

				for (auto msg = msgs.begin(); msg != msgs.end();
						++msg) {
					servlet.handle_msg(*msg, end);
				}
				em_it->second.clear();
			}

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
