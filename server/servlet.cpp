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

void handle_newusers(Servlet servlet)
{
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
		while (!killself) { // make an atomic check.			h
			bool check = check_newusers(servlet.get_chan());
			if (check)
				handle_newusers(servlet);

			check = check_sockmsgs(servlet);
			if (!check)
				continue;

			std::map<tcp::socket, std::deque<std::string>> local_sockmsgs;
			local_sockmsgs = copy_sockmsgs(servlet);

			handle_sockmsgs(local_sockmsgs, servlet);
		}
	} catch (...) {
		// @TODO: check if child threads can catch keyboard exceptions etc.
		// since we want the parent thread to handle that stuff.
		// Might need a signal handler?
		// @TODO: make another catch for catcheable exceptions.
		std::cout << "There was an error" << std::endl;
	}
}
