/*
 * server.cpp
 *
 * Zeke Reyna
 *
 * @TODO: sock_io ops need proper timeout mechanisms so doesn't block forever
 * or make actually async
 */

#include "server.h"

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
	return try_reading_from_sock(sock, sock_msgs);
}

void Server::try_writing(tcp::socket& sock, std::string msg)
{
	try_writing_to_sock(sock, msg);
}

User Server::register_session(tcp::socket& sock)
{
	std::string msg = try_reading(sock);
	// "NICK <nick>"
	std::string nick = msg.substr(5, std::string::npos);

	msg = try_reading(sock);
	// "USER <user-name> * * :<real-name>"
	std::deque<std::string> parts = split_(msg, " * * :");

	std::string part1, part2;
	part1 = parts.front();
	part2 = parts.back();
	std::string user_name, real_name;
	user_name = part1.substr(5, std::string::npos);
	real_name = part2;

	User client(nick, user_name, real_name);
	client.set_endpoint(sock.remote_endpoint());
	// send "<this-IP> 001 <nick> :Welcome to the Internet
	// Relay Network <nick>!<user>@<their-IP>\r\n"
	std::string rem_IP = sock.remote_endpoint().address().to_string();
	std::string loc_IP = sock.local_endpoint().address().to_string();

	msg  = loc_IP + " " + RPL_WELCOME + " " + nick + " :Welcome to"
	     + " the Internet Relay Network " + nick + "!" + user_name
	     + "@" + rem_IP + "\r\n";
	try_writing(sock, msg);

	return client;
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

void Server::inner_scope_run(boost::asio::io_service& io_service,
		tcp::acceptor& acceptor)
{
	tcp::socket sock(io_service);
	boost::system::error_code ec;
	acceptor.accept(sock, ec);
	if (ec == boost::asio::error::would_block)
		return;

	User client (register_session(sock));
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
		std::thread thr(thread_run,
				channel,
				&_chan_newusers_map,
				&_socks_deq,
				&_comms_lock,
				_notify_of_newusers.pass_ptr(channel));
		_threads[channel] = std::move(thr);
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

		boost::asio::io_service io_service;
		tcp::acceptor acceptor(io_service,
				tcp::endpoint(tcp::v4(), listen_port));
		acceptor.non_blocking(true);

		while (!killself) {
			inner_scope_run(io_service, acceptor);
		}

		std::cout << "\ngot signal to killself, going to cleanup _threads\n";
		for (auto& t : _threads)
			t.second.join();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << "\n";

		killself = true; // atomic
		for (auto& t : _threads)
			t.second.join();
	}
}
