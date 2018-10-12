/*
 * server.h
 *
 * Zeke Reyna
 *
 * @TODO: give descriptions for each method
 */

#ifndef __SERVER_H__
#define __SERVER_H__

#include "globals.h"
#include "Servlet.h"
#include <User.h>
#include <sockio.h>
#include "Notifier.h"

#include <iostream>
#include <string>
#include <map>
#include <array>
#include <deque>
#include <mutex>
#include <thread>
#include <tuple>
#include <atomic>
#include <asio.hpp>
#include <chitter.h>
#include <pqxx/pqxx>
#include <optional>

// some macros for configurable values
#define DEFAULT_TOPIC "DEFAULT_TOPIC"

using tcp = asio::ip::tcp;

class Server {

	public:

        Server() = delete;

        Server(const std::string serverName)
        :_SERVER_NAME{serverName}{ }

        //@TODO: get rid of other constructors / etc.

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
		std::optional<User> register_session(tcp::socket& sock);

		/*
		 */
		std::string get_channel_name(tcp::socket& sock);

		/*
		 */
		void update_comm_structs(User& u, std::deque<std::string>& msgs,
				tcp::socket& sock);

		/*
		 */
		void inner_scope_run(asio::io_service& io_service,
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

		// for db querying
		pqxx::connection _connection = chitter::initiate("../shared/config");

		const std::string _SERVER_NAME;
};

#endif
