/*
 * server.h
 *
 * Zeke Reyna
 *
 * @TODO: give descriptions for each method
 */

#ifndef __SERVER_H__
#define __SERVER_H__

#include "consts_globs_shared.h"
#include "servlet.h"
#include "user.h"
#include "../libs/sockio.h"
#include "notifier.h"

#include <iostream>
#include <string>
#include <map>
#include <array>
#include <deque>
#include <mutex>
#include <thread>
#include <tuple>
#include <atomic>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class Server {

	public:
		/*
		 */
		void set_globals() { killself = false; }

		/*
		 */
		std::string try_reading(tcp::socket& sock);

		/*
		 */
		void try_writing(tcp::socket& sock, std::string msg);

		/*
		 */
		User register_session(tcp::socket& sock);

		/*
		 */
		std::string get_channel_name(tcp::socket& sock);

		/*
		 */
		void update_comm_structs(User& u, std::deque<std::string>& msgs,
				tcp::socket& sock);

		/*
		 */
		void inner_scope_run(boost::asio::io_service& io_service,
				tcp::acceptor& acceptor);

		/*
		 */
		void run(int listen_port);

	private:
		std::map<tcp::endpoint, std::deque<std::string>> _end_msgs;

		// these are for passing info to threads
		std::map<std::string, std::deque<std::tuple<User, std::deque<
			std::string>>>> _chan_newusers_map;

		std::deque<tcp::socket> _socks_deq;

		std::mutex _comms_lock;

		Notifier _notify_of_newusers;
		// these are for passing info to threads

		std::map<std::string, std::thread> _threads;

};

#endif
