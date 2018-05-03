/*
 * user.h
 *
 * Zeke Reyna
 *
 * This class holds information that can be used to differentiate each user
 * connected the server.
 * The _endpt member is in fact a remote_endpoint, which is used for grabbing
 * the correct socket when info needs to be passed to the host associated with
 * this user.
 *
 * @TODO: change as much as possible to const args, or const rettypes
 * @TODO: implement move constructor? Mght be worth it for endpoint...
 */
#ifndef __USER_H__
#define __USER_H__

#include <string>
//#include <boost/asio.hpp>
#include <asio.hpp>

using tcp = asio::ip::tcp;

// could really just be a struct instead of a class...
class User
{
	public:
		// default constructor
		User()
		{
			_nick      = "";
			_user_name = "";
			_real_name = "";
			_password  = "";
			_channel   = "";
		}

		// constructor with params
		User(const std::string& n, const std::string& u, const std::string& r)
			:_nick{n}, _user_name{u}, _real_name{r}
		{
			_password = "";
			_channel  = "";
		}

		// copy constructor
		User(const User& other)
		{
			_nick      = other._nick;
			_user_name = other._user_name;
			_real_name = other._real_name;
			_password  = other._password;
			_channel   = other._channel;
			_endpt     = other._endpt;
		}


		// destructor
		~User() = default;

		// member functions
		void set_nick(std::string new_nick)    { _nick = new_nick; }
		void set_pass(std::string new_pass)    { _password = new_pass; }
		void set_channel(std::string new_chan) { _channel = new_chan; }
		void set_endpoint(const tcp::endpoint new_e)
		{
			_endpt = new_e;
		}

		std::string get_nick() const { return _nick; 	  }
		std::string get_user() const { return _user_name; }
		std::string get_real() const { return _real_name; }
		std::string get_pass() const { return _password;  }
		std::string get_chan() const { return _channel;   }
		tcp::endpoint get_endpt() const { return _endpt; }

		std::string print_() const
		{
			std::string msg;
			msg += "nick: " + _nick + "\n";
			msg += "user: " + _user_name + "\n";
			msg += "real: " + _real_name + "\n";
			msg += "channel: " + _channel + "\n";
			msg += "IP: " + _endpt.address().to_string() + "\n";
			return msg;
		}

	private:
		std::string _nick;
		std::string _user_name;
		std::string _real_name;
		std::string _password;
		std::string _channel;
		tcp::endpoint _endpt; // note: remote endpoint.
};

#endif
