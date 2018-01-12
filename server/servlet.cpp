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

std::deque<User> grab_newusers(Servlet& servlet)
{
	try {
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
	} catch (...) {
		throw;
	}
}

void handle_newusers(Servlet& servlet)
{
	try {
		std::deque<User> newusers = grab_newusers(servlet);
		for (auto it = newusers.begin(); it != newusers.end(); ++it) {
		}

		// now that info is grabbed, need to handle it.
	} catch (...) {
		throw;
	}
}

bool check_newusers(std::string chan)
{
	std::unique_lock<std::mutex> lck(newusers_lock);
	if (chan_newusers.count(chan) && !chan_newusers[chan].empty())
		return true;
	else
		return false;
}

bool check_sockmsgs(Servlet servlet)
{
	std::unique_lock<std::mutex> lck(msgs_lock);
	for (auto it = servlet.get_dict().begin(); it != servlet.get_dict().end();
									++it) {
		if (!sock_msgs[it->first].empty())
			return true;
	}
	return false;
}

std::map<tcp::socket, std::deque<std::string>> copy_sockmsgs(Servlet servlet)
{
	std::unique_lock<std::mutex> lck(msgs_lock);
	std::map<tcp::socket, std::deque<std::string>> retval;
	for (auto sock = servlet.get_dict().begin(); sock != servlet.get_dict().end();
									++sock) {
		for (auto q_it = sock_msgs[sock->first].begin();
				q_it!= sock_msgs[sock->first].end();
				++q_it) {
			retval[sock->first].push_back(*q_it);
		}
		sock_msgs[sock->first].clear();
	}
	return retval;
}

void handle_msg(std::string msg, tcp::socket& sock, User user)
{
}

void handle_sockmsgs(std::map<tcp::socket, std::deque<std::string>> local_sockmsgs,
		Servlet servlet)
{
	for (auto sockdeq = local_sockmsgs.begin(); sockdeq != local_sockmsgs.end();
			++sockdeq) {
		for (auto msg = sockdeq->second.begin(); msg != sockdeq->second.end();
				++msg) {
			User this_client = servlet.get_dict()[sockdeq->first];
			tcp::socket& sock = sockdeq->first;
			handle_msg(*msg, sockdeq->first, this_client);
		}
	}
}

void run(Servlet servlet)
{
	try {
		while (!killself) {
			bool check = check_newusers(servlet.get_chan());
			if (check)
				handle_newusers(servlet);

			check = check_sockmsgs(servlet);
			if (!check)
				continue;

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
