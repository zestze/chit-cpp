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

// ************ GLOBALS ****************
std::atomic<bool> killself;
// ************ GLOBALS ****************

void signal_handler(int signal)
{
	if (signal)
		killself = true;
}

std::string Server::try_reading(tcp::socket& sock)
{
	tcp::endpoint end = sock.remote_endpoint();
	if (!_end_msgs.count(end))
		_end_msgs[end]; // instantiate bc i'm nervous
	std::deque<std::string>& sock_msgs = _end_msgs[end];
	return sockio::try_reading_from_sock(sock, sock_msgs);
}

void Server::try_writing(tcp::socket& sock, std::string msg)
{
	sockio::try_writing_to_sock(sock, std::move(msg));
}

std::optional<User> Server::register_session(tcp::socket& sock)
{
	std::string msg = try_reading(sock);
	// "NICK <nick>"
	std::string nick = msg.substr(5, std::string::npos);

	msg = try_reading(sock);
	// "USER <user-name> * * :<real-name>"
	std::deque<std::string> parts = sockio::split(msg, " * * :");

	msg = try_reading(sock);
	// "PASS <password>"
	std::string password = msg.substr(5, std::string::npos);

	std::string part1, part2;
	part1 = parts.front();
	part2 = parts.back();
	std::string user_name, real_name;
	user_name = part1.substr(5, std::string::npos);
	real_name = part2;

	User client(nick, user_name, real_name, password);
	client.set_endpoint(sock.remote_endpoint());

	std::optional<User> clientOp;

	const bool USER_HANDLING_SUCCESSFUL = chitter::handleUser(client, _connection);
	std::string replyCode;
	std::string replyMessage;
	if (!USER_HANDLING_SUCCESSFUL) {
	    // send "<this-IP> 464 <nick> :Password incorrect <nick>!<user>@<their-IP>\r\n"
	    replyCode = ERR_PASSWDMISMATCH;
	    replyMessage = ":Password incorrect";

	    // set our return value to let caller know that handling was not successful
	    clientOp = std::nullopt;
	}
	else {
	    // send "<this-IP> 001 <nick> :Welcome to the Internet
        // Relay Network <nick>!<user>@<their-IP>\r\n"
        replyCode = RPL_WELCOME;
        replyMessage = ":Welcome to the Internet Relay Network";

        // set our return value to let caller know that handling was successful
        clientOp = std::optional<User>(client);

        // also need to register loginInstance for this user
        chitter::insertLogin(client.get_nick(), sock.remote_endpoint(), _connection);
	}

    std::string rem_IP = sock.remote_endpoint().address().to_string();
    std::string loc_IP = sock.local_endpoint().address().to_string();

    msg  = loc_IP + " " + replyCode + " " + nick + " " + replyMessage
           + " " + nick + "!" + user_name
           + "@" + rem_IP + "\r\n";
    try_writing(sock, msg);

	return clientOp;
}

std::string Server::get_channel_name(tcp::socket& sock)
{
	std::string msg = try_reading(sock);
	std::string channel = msg.substr(5, std::string::npos);
	return channel;
}

void Server::update_comm_structs(User& u, std::deque<std::string>& msgs,
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
	tcp::socket sock(io_service);
	asio::error_code ec;
	acceptor.accept(sock, ec);
	if (ec == asio::error::would_block)
		return;
	asio::socket_base::keep_alive option(true);
	sock.set_option(option);

	std::optional<User> clientOpt = register_session(sock);
	if (!clientOpt) {
	    // don't handle this user
	    return;
	}
	User client (*clientOpt);

	std::string channel (get_channel_name(sock));
	client.set_channel(channel);

	// move any received messages over, just in case there are any
	// added reference for clarity
	auto& client_msgs = _end_msgs[sock.remote_endpoint()];
	std::deque<std::string> temp_msgs (client_msgs);
	_end_msgs.erase(sock.remote_endpoint());

	update_comm_structs(client, temp_msgs, sock);
	// don't user sock after this

	_notify_of_newusers.update(channel);

	if (!_threads.count(channel)) {
	    // spin up a thread to represent the created 'channel'
	    //@TODO: make it so that topic is not default
	    //@TODO: make it cleaner, so that less args need to be passed.
	    //auto strings = std::make_tuple(_SERVER_NAME, channel, DEFAULT_TOPIC);
	    //auto pointers = std::make_tuple(&_chan_newusers_map, &_socks_deq, &_comms_lock);
	    //std::thread thr(thread_run,
	    //		strings,
	    //		pointers,
	    //		_notify_of_newusers.pass_ptr(channel));
		std::thread thr(thread_run,
				_SERVER_NAME,
				channel,
				DEFAULT_TOPIC,
				&_chan_newusers_map,
				&_socks_deq,
				&_comms_lock,
				_notify_of_newusers.pass_ptr(channel));
		_threads[channel] = std::move(thr);

		// make sure to log into database
		chitter::insertChannel(channel, DEFAULT_TOPIC, _SERVER_NAME);
	}
}

void Server::run(int listen_port)
{
	try {
		std::ios_base::sync_with_stdio(false);
		std::cout << "Starting server...\n";
		std::cout << "Type CTRL+C to quit" << std::endl;

		//set_globals();
		set_globals();
		std::signal(SIGINT, signal_handler);

		asio::io_service io_service;
		tcp::acceptor acceptor(io_service,
				tcp::endpoint(tcp::v4(), listen_port));
		acceptor.non_blocking(true);

		//@TODO: make this servername an argument that gets passed by
		//@TODO: whatever calls run()
		const std::string SERVER_NAME = "testServer";
		chitter::handleServer(SERVER_NAME, acceptor.local_endpoint());

		while (!killself) {
			inner_scope_run(io_service, acceptor);
		}

		std::cout << "\ngot signal to killself, going to cleanup _threads\n";
		for (auto& t : _threads)
			t.second.join();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << "\n";

		killself = true; // atomic
		for (auto& t : _threads)
			t.second.join();
	}
}
