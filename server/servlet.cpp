/*
 * servlet.cpp
 *
 * Zeke Reyna
 *
 * @TODO: all need to be wrapped in try / catch?
 */

#include "servlet.h"
#include <iostream>
#include <deque>
#include <map>
#include <boost/algorithm/string.hpp>

// @TODO: changed part of this function, but didn't finish with changes.
std::string try_reading_from_sock(Servlet& servlet, User user)
{
	tcp::endpoint end = user.get_endpt();
	tcp::socket& sock = servlet.end_socks[end];
	if (servlet.end_socks.count(end) && !servlet.end_msgs[end].empty()) {
		std::string msg = servlet.end_msgs[end].front();
		servlet.end_msgs[end].pop_front();
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
			servlet.end_msgs[end].push_back(*it);
	}

	std::string msg = servlet.end_msgs[end].front();
	servlet.end_msgs[end].pop_front();
	return msg;
}

void try_writing_to_sock(tcp::socket& sock, std::string msg)
{
	if (msg.substr(msg.length() - 2, std::string::npos) != "\r\n")
		throw std::invalid_argument("All IRC msgs need \\r\\n suffix");
	boost::system::error_code ec;
	boost::asio::write(sock, boost::asio::buffer(msg),
			boost::asio::transfer_all(), ec);
	// @TODO: implement proper async write, or have a timeout.
	// this blocks until all in buffer is transmitted.
}

// change name?
std::deque<tcp::endpoint> update_end_msgs(Servlet& servlet)
{
	std::deque<tcp::endpoint> read_ends;
	// note: it is a pair of <endpoint, socket>
	for (auto es_it = servlet.end_socks.begin(); es_it != servlet.end_socks.end(); ++es_it) {
		std::size_t len = es_it->second.available();
		if (len <= 0)
			continue;
		read_ends.push_back(es_it->first);

		tcp::endpoint end = es_it->first;
		tcp::socket& sock = es_it->second;

		std::vector<char> buff;
		boost::system::error_code ec;
		sock.read_some(boost::asio::buffer(buff), ec);
		std::string full_msg(buff.begin(), buff.end());
		std::vector<std::string> msgs;

		boost::algorithm::split(msgs, full_msg, boost::is_any_of("\r\n"));
		for (auto it = msgs.begin(); it != msgs.end(); ++it) {
			if (*it != "")
				servlet.end_msgs[end].push_back(*it);
		}
	}
	return read_ends;
}

std::deque<User> grab_newusers(Servlet& servlet)
{
	std::unique_lock<std::mutex> lck(newusers_lock);
	std::string chan = servlet.get_chan();
	std::deque<User> newusers;
	for (auto it = chan_newusers[chan].begin(); it != chan_newusers[chan].end(); ++it) {
		// copy over user
		User new_u = std::get<0>(*it);
		servlet.add_user(new_u);

		// copy over sock
		tcp::socket temp(std::move(std::get<1>(*it))); // @TODO: check if move works
		servlet.end_socks[new_u.get_endpt()];
		servlet.end_socks[new_u.get_endpt()] = std::move(temp); // @TODO: see above

		// copy over messages
		servlet.end_msgs[new_u.get_endpt()] = std::get<2>(*it);

		// for later handling of new users
		newusers.push_back(new_u);
	}
	return newusers;
}

void handle_newusers(Servlet& servlet)
{
	std::deque<User> newusers = grab_newusers(servlet);
	std::deque<User> handled;
	for (auto it = newusers.begin(); it != newusers.end(); ++it) {
		User new_user = *it;
		std::string rem_IP = new_user.get_endpt().address().to_string();
		std::string msg;
		msg  = new_user.get_nick() + "!" + new_user.get_user() + "@" + rem_IP;
		msg += " JOIN " + new_user.get_chan() + "\r\n";

		std::string user_names = "";
		for (auto it2 = servlet.users.begin(); it2 != servlet.users.end(); ++it) {
			// if user is in newusers, but not in handled, it shouldn't
			// be included in curr list of users, and shouldn't be messsaged
			// about this new person.
			if (std::find(newusers.begin(), newusers.end(), *it2) != newusers.end()
			&& std::find(handled.begin(), handled.end(), *it2) == handled.end())
				continue;
			tcp::endpoint end = it2->get_endpt();
			try_writing_to_sock(servlet.end_socks[end], msg);
			//@TODO: make own, try_writing_to_sock for this servlet
			
			user_names += "@" + it2->get_nick() + " ";
		}

		std::string loc_IP;
		loc_IP = servlet.end_socks[new_user.get_endpt()].local_endpoint().address().to_string();
		msg  = loc_IP + " " + RPL_TOPIC + " ";
		msg += new_user.get_nick() + " " + new_user.get_chan() + " ";
		msg += ":" + servlet.get_topic() + "\r\n";
		try_writing_to_sock(servlet.end_socks[new_user.get_endpt()], msg);

		msg  = loc_IP + " " + RPL_NAMREPLY + " ";
		msg += new_user.get_nick() + " " + new_user.get_chan() + " ";
		msg += ":" + user_names + "\r\n";
		try_writing_to_sock(servlet.end_socks[new_user.get_endpt()], msg);

		msg  = loc_IP + " " + RPL_ENDOFNAMES + " ";
		msg += new_user.get_nick() + " " + new_user.get_chan() + " ";
		msg += ":End of NAMES list\r\n";
		try_writing_to_sock(servlet.end_socks[new_user.get_endpt()], msg);

		// new_user has been handleed, so make sure to put them in that deque
		handled.push_back(new_user);
	}

	// now that info is grabbed, need to handle it.
}

// return TRUE if needs handling
bool check_newusers(std::string chan)
{
	std::unique_lock<std::mutex> lck(newusers_lock);
	if (chan_newusers.count(chan) && !chan_newusers[chan].empty())
		return true;
	else
		return false;
}

// return portion of end_msgs that needs handling
bool check_end_msgs(Servlet& servlet)
{
	for (auto it = servlet.end_msgs.begin(); it != servlet.end_msgs.end(); ++it) {
		if (!it->second.empty())
			return true;
	}
	return false;
}

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
			tcp::socket& sock = servlet.end_socks[it->get_endpt()];
			try_writing_to_sock(sock, msg);
		}
	} else if (msg.substr(0, 4) == "PART") {
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
			tcp::socket& sock = servlet.end_socks[it->get_endpt()];
			try_writing_to_sock(sock, msg);
		}

		// remove user from users deque
		// del entry for end_msgs and end_socks
		for (auto it = servlet.users.begin(); it != servlet.users.end(); ++it) {
			if (it->get_endpt() == end) {
				servlet.users.erase(it);
				break;
			}
		}

		servlet.end_msgs.erase(end);
		servlet.end_socks.erase(end); // calling erase like this should implicitly close socket
	} else {
		std::cout << "Unrecognized Message:" << std::endl;
		std::cout << msg << std::endl;
		throw std::invalid_argument("Unrecognized message format");
	}
}

void run(Servlet servlet)
{
	try {
		while (!killself) {
			bool check = check_newusers(servlet.get_chan());
			if (check)
				handle_newusers(servlet);

			//check = check_end_msgs(servlet);
			//if (!check)
				//continue;
			update_end_msgs(servlet); // kind fo don't need return value...
			// because there can be other msgs that need handling.
			// Really just need to loop over end_msgs

			// Handle end_msgs
			for (auto em_it = servlet.end_msgs.begin(); em_it != servlet.end_msgs.end();
					++em_it) {
				for (auto msg = em_it->second.begin(); msg != em_it->second.end();
						++msg) {
					handle_msg(*msg, servlet, em_it->first);
				}
				em_it->second.clear();
			}

		}

		// clean up, don't need to close sockets since map and deque should do
		// implicitly by destructor call.
		std::cout << "Thread exiting" << std::endl;
	} catch (...) {
		// @TODO: check if child threads can catch keyboard exceptions etc.
		// since we want the parent thread to handle that stuff.
		// Might need a signal handler?
		// @TODO: make another catch for catcheable exceptions.
		std::cout << "There was an error" << std::endl;
	}
}
