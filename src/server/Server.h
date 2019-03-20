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
#include <sockio/sockio.h>
#include "Notifier.h"
#include <configUtils/configUtils.h>

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
#include <chitter/chitter.h>
#include <pqxx/pqxx>

using tcp = asio::ip::tcp;

class Server {

	using UserSuccessPair = std::pair<User, bool>;
	using MsgList = std::deque<std::string>;
	using SocketList = std::deque<tcp::socket>;

	public:


    //@TODO: store reference to ServerConfig instead of topic and name
        // *******************
        // *******************
        Server() :
        _serverConfig(configUtils::loadServerConfig()),
        _connection(chitter::initiate()) {
        }

        // *******************
        // *******************
        ~Server() {
            for (auto& t : _threads) 
                t.second.join(); 
            // pqxx and asio members clean up their
            // own resources
        }

        // *******************
        // *******************
		void set_globals() { selfdestruct = false; }

		/*
		 */
		std::string try_reading(tcp::socket& sock);

		/*
		 */
		void try_writing(tcp::socket& sock, std::string msg);

        /*
         */
        std::tuple<bool, std::string> hashPassword(std::string password);

		/*
		 */
		UserSuccessPair register_session(tcp::socket& sock);

		/*
		 */
		std::string get_channel_name(tcp::socket& sock);

		/*
		 */
		void update_comm_structs(User& u, MsgList& msgs,
				tcp::socket& sock);

		/*
		 */
		void inner_scope_run(asio::io_service& io_service,
				tcp::acceptor& acceptor);

		/*
		 */
		void run(int listen_port);

	private:
		std::map<tcp::endpoint, MsgList> _end_msgs;

		// these are for passing info to threads
		std::map<std::string, std::deque<std::tuple<User,
		    MsgList>>> _chan_newusers_map;

		SocketList _socks_deq;

		std::mutex _comms_lock;

		Notifier _notify_of_newusers;
		// these are for passing info to threads

		std::map<std::string, std::thread> _threads;

		// for db querying
		pqxx::connection _connection;

		const configUtils::ServerConfig _serverConfig;
};

#endif
