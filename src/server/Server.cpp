/*
 * server.cpp
 *
 * Zeke Reyna
 *
 * @TODO: sock_io ops need proper timeout mechanisms so doesn't block forever
 * or make actually async
 */

#include "Server.h"
#include <utility>
#include <ircConstants.h>
#include <openssl/sha.h>
#include <stdlib.h>

// ************ GLOBALS ****************
std::atomic<bool> selfdestruct;
// ************ GLOBALS ****************

void signal_handler(int signal)
{
	if (signal)
		selfdestruct = true;
}

std::string Server::try_reading(tcp::socket& sock)
{
	const tcp::endpoint END_POINT = sock.remote_endpoint();
	MsgList& sock_msgs = _end_msgs[END_POINT];
	return sockio::try_reading_from_sock(sock, sock_msgs);
}

void Server::try_writing(tcp::socket& sock, std::string msg)
{
	sockio::try_writing_to_sock(sock, std::move(msg));
}

std::tuple<bool, std::string> Server::hashPassword(std::string password)
{
    unsigned char md[SHA256_DIGEST_LENGTH] = ""; // zero initialize

    SHA256_CTX context;
    const bool success = SHA256_Init(&context) &&
        SHA256_Update(&context, password.data(), password.length()) &&
        SHA256_Final(md, &context);

    // this will cause issues across different architectures / implementations
    // since the diff between unsigned char and char can vary
    auto convertChar = [] (unsigned char c) -> char {
        if (c > std::numeric_limits<char>::max()) {
            throw std::runtime_error("during password hashing, encountered size issue");
        }
        return c;
    };
    std::string hash (SHA256_DIGEST_LENGTH, '0');
    std::transform(std::begin(md), std::end(md), hash.begin(), convertChar);

    return std::make_tuple(success, hash);
}

Server::UserSuccessPair Server::register_session(tcp::socket& sock)
{
	std::string msg = try_reading(sock);
	// "NICK <nick>"
	std::string nick = msg.substr(5, std::string::npos);

	msg = try_reading(sock);
	// "USER <user-name> * * :<real-name>"
	MsgList parts = sockio::split(msg, " * * :");

	msg = try_reading(sock);
	// "PASS <password>"
	std::string password = msg.substr(5, std::string::npos);
    // hash the password
    auto [success, hash] = hashPassword(password);

	const std::string PART1 = parts.front();
	const std::string PART2 = parts.back();
	const std::string USER_NAME = PART1.substr(5, std::string::npos);
	const std::string REAL_NAME = PART2;

    //@TODO: need to reset passwords in the database...
	User client(nick, USER_NAME, REAL_NAME, hash);
	client.set_endpoint(sock.remote_endpoint());

	const bool USER_HANDLING_SUCCESSFUL = chitter::handleUser(client, _connection);
	std::string replyCode;
	std::string replyMessage;
	if (!USER_HANDLING_SUCCESSFUL) {
	    // send "<this-IP> 464 <nick> :Password incorrect <nick>!<user>@<their-IP>\r\n"
	    replyCode = ERR_PASSWDMISMATCH;
	    replyMessage = ":Password incorrect";

	}
	else {
	    // send "<this-IP> 001 <nick> :Welcome to the Internet
        // Relay Network <nick>!<user>@<their-IP>\r\n"
        replyCode = RPL_WELCOME;
        replyMessage = ":Welcome to the Internet Relay Network";

        // also need to register loginInstance for this user
        chitter::insertLogin(client.get_nick(), sock.remote_endpoint(), _connection);
	}

    const std::string REM_IP = sock.remote_endpoint().address().to_string();
    const std::string LOC_IP = sock.local_endpoint().address().to_string();

    msg  = LOC_IP + " " + replyCode + " " + nick + " " + replyMessage
           + " " + nick + "!" + USER_NAME
           + "@" + REM_IP + "\r\n";
    try_writing(sock, msg);

	return {client, USER_HANDLING_SUCCESSFUL};
}

std::string Server::get_channel_name(tcp::socket& sock)
{
	const std::string MSG = try_reading(sock);
	const std::string CHANNEL = MSG.substr(5, std::string::npos);
	return CHANNEL;
}

void Server::update_comm_structs(User& u, MsgList& msgs,
		tcp::socket& sock)
{
	std::lock_guard<std::mutex> lock(_comms_lock);
	_chan_newusers_map[u.get_chan()].push_back(std::make_tuple(u, msgs));
	_socks_deq.push_back(std::move(sock));
	// don't use sock after this
}

void Server::inner_scope_run(asio::io_service& io_service,
		tcp::acceptor& acceptor)
{
	tcp::socket sock (io_service);
	asio::error_code ec;
	acceptor.accept(sock, ec);
	if (ec == asio::error::would_block)
		return;
	asio::socket_base::keep_alive option(true);
	sock.set_option(option);

	auto [client, success] = register_session(sock);
	if (!success) {
	    // don't handle this user
	    return;
	}

	const std::string CHANNEL (get_channel_name(sock));
	client.set_channel(CHANNEL);

	// move any received messages over, just in case there are any
	// added reference for clarity
	MsgList& client_msgs = _end_msgs[sock.remote_endpoint()];
	MsgList temp_msgs (client_msgs);
	_end_msgs.erase(sock.remote_endpoint());

	update_comm_structs(client, temp_msgs, sock);
	// don't user sock after this

	_notify_of_newusers.update(CHANNEL);

	if (!_threads.count(CHANNEL)) {

		// make sure to log into database
		// @TODO: for now, only inserting if doesn't already exist. in future,
		// @TODO: need a more defined mechanism.
		std::string channelTopic = _serverConfig.defaultTopic;
		const std::string serverName = _serverConfig.defaultName;
		const bool CHANNEL_EXISTS = chitter::checkChannelExists(CHANNEL, serverName, _connection);
		if (CHANNEL_EXISTS) {
			channelTopic = chitter::getChannelTopic(CHANNEL, serverName, _connection);
		} else {
			chitter::insertChannel(CHANNEL, channelTopic, serverName, _connection);
		}

	    // spin up a thread to represent the created 'channel'
	    //@TODO: make it so that topic is not default
	    //@TODO: make it cleaner, so that less args need to be passed.
	    //auto strings = std::make_tuple(serverName, channel, channelTopic);
	    //auto pointers = std::make_tuple(&_chan_newusers_map, &_socks_deq, &_comms_lock);
	    //std::thread thr(thread_run,
	    //		strings,
	    //		pointers,
	    //		_notify_of_newusers.pass_ptr(channel));
		std::thread thr(thread_run,
				serverName,
				CHANNEL,
				channelTopic,
				&_chan_newusers_map,
				&_socks_deq,
				&_comms_lock,
				_notify_of_newusers.pass_ptr(CHANNEL));
		_threads[CHANNEL] = std::move(thr);

	}
}

void Server::run(int listen_port)
{
	try {
	    // **********************************
	    // don't sync c and c++ buffers
	    // also, let user now the program
	    // is loading up
		// **********************************
		std::ios_base::sync_with_stdio(false);
		std::cout << "Starting server...\n"
		<< "Type CTRL+C to quit" << std::endl;

		// **********************************
		// need to register globals and signal
		// handler, for proper multi-threading
		// **********************************
		set_globals();
		std::signal(SIGINT, signal_handler);

		// **********************************
		// setup accepting (listening) socket
		// **********************************
		asio::io_service io_service;
		tcp::acceptor acceptor(io_service,
				tcp::endpoint(tcp::v4(), listen_port));
		acceptor.non_blocking(true);

		// **********************************
		// create a new server in the database if one does not exist.
		// regardless, insert a serverMetaData instance
		// **********************************
		chitter::handleServer(_serverConfig.defaultName,
				acceptor.local_endpoint(), _connection);

		// **********************************
		// the main loop of the program
		// **********************************
		while (!selfdestruct) {
			inner_scope_run(io_service, acceptor);
		}

		// **********************************
		// notify user that program is exiting,
		// and then clean up threads
		// **********************************
		std::cout << "\ngot signal to selfdestruct, going to cleanup _threads\n";
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << "\n";

		selfdestruct = true; // atomic
	}
}
