/*
 * server.cpp
 *
 * Zeke Reyna
 *
 * @TODO: sock_io ops need proper timeout mechanisms so doesn't block forever
 * or make actually async
 */

#include "server.h"

/*      	INIT GLOBALS 		*/
std::string RPL_WELCOME    = "001";
std::string RPL_TOPIC      = "332";
std::string RPL_NAMREPLY   = "252";
std::string RPL_ENDOFNAMES = "366";

std::atomic<bool> killself(false);

std::map<std::string, std::deque<std::tuple<User, std::deque<std::string>>>> chan_newusers;

std::deque<tcp::socket> global_socks;
std::mutex c_nu_lock;
std::mutex g_s_lock;
/*      	INIT GLOBALS 		*/


// @TODO: changed part of this function, but didn't finish with changes.
std::string try_reading_from_sock(tcp::socket& sock)
{
	//std::unique_lock<std::mutex> lock(msgs_lock);
	tcp::endpoint end = sock.remote_endpoint();
	if (end_msgs.count(end) && !end_msgs[end].empty()) {
		std::string msg = end_msgs[end].front();
		end_msgs[end].pop_front();
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
			end_msgs[end].push_back(*it);
	}

	// @TODO:
	// honestly can grab first thing from adding in for loop
	// don't add, just return. Will be smoother.
	std::string msg = end_msgs[end].front();
	end_msgs[end].pop_front();
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

User register_session(tcp::socket& sock)
{
	std::string msg = try_reading_from_sock(sock);
	// "NICK <nick>"
	std::string nick = msg.substr(5, std::string::npos);

	msg = try_reading_from_sock(sock);
	// "USER <user-name> * * :<real-name>"
	std::deque<std::string> parts;
	boost::algorithm::split(parts, msg, boost::is_any_of(" * * :"));
	std::string part1, part2;
	part1 = parts.front();
	part2 = parts.back();
	std::string user_name, real_name;
	user_name = part1.substr(5, std::string::npos);
	real_name = part2;

	User client(nick, user_name, real_name);
	// send "<this-IP> 001 <nick> :Welcome to the Internet
	// Relay Network <nick>!<user>@<their-IP>\r\n"
	std::string rem_IP = sock.remote_endpoint().address().to_string();
	std::string loc_IP = sock.local_endpoint().address().to_string();

	msg  = loc_IP + " " + RPL_WELCOME + " " + nick + " :Welcome to";
	msg += " the Internet Relay Network " + nick + "!" + user_name;
	msg += "@" + rem_IP + "\r\n";
	try_writing_to_sock(sock, msg);

	return client;
}

std::string get_channel_name(tcp::socket& sock)
{
	std::string msg = try_reading_from_sock(sock);
	std::string channel = msg.substr(5, std::string::npos);
	return channel;
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		std::cout << "Usage: ./server <server-port>" << std::endl;
		return -1;
	}
	std::cout << "Starting server..." << std::endl;
	int listen_port = std::stoi(argv[1]);

	//init_globals();

	std::map<std::string, std::thread> threads;
	try
	{
		boost::asio::io_service io_service;
		tcp::acceptor acceptor(io_service,
				tcp::endpoint(tcp::v4(), listen_port));

		for (;;) {
			tcp::socket sock(io_service);
			acceptor.accept(sock);

			std::cout << "Got a connection at listening port\n";
			User client = register_session(sock);
			client.set_endpoint(sock.remote_endpoint());

			std::string channel = get_channel_name(sock);
			client.set_channel(channel);

			client.set_endpoint(sock.remote_endpoint());

			std::deque<std::string> temp_msgs = end_msgs[sock.remote_endpoint()];
			end_msgs.erase(sock.remote_endpoint());

			{
				std::unique_lock<std::mutex> lck(c_nu_lock);
				if (!chan_newusers.count(channel))
					chan_newusers[channel]; // initialize
				chan_newusers[channel].push_back(std::make_tuple(client,
							temp_msgs));
				// @TODO: error likely here, since std::move(sock)
				// is right, but then push_back in deque might try
				// to coopy sock by value and cause error
				// so, if this doesn't work, make an extra map
				// with same key, but the value will be a deque of
				// sockets. then do push_back(std::move(sock))
			}

			{
				std::unique_lock<std::mutex> lck(g_s_lock);
				global_socks.push_back(std::move(sock));
				// @TODO: push_back vs emplace_back ?
			}

			if (!threads.count(channel)) {
				//Servlet servlet(channel);
				//std::thread thr(run, servlet);
				std::thread thr(run, channel);
				threads[channel] = std::move(thr);
			}
		}
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;

		killself = true; // atomic
		for (auto it = threads.begin(); it != threads.end(); ++it)
			it->second.join(); // might not work??? it's a pointer so maybe... but no copy constr.
		return -1;
	}
	catch (...)
	{
		std::cout << "Unrecognized error" << std::endl;
		for (auto it = threads.begin(); it != threads.end(); ++it)
			it->second.join();
		return -1;
	}
	return 0;
}
