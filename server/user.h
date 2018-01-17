/*
 * user.h
 *
 * Zeke Reyna
 */
#ifndef __USER_H__
#define __USER_H__

#include <string>
#include <boost/asio.hpp>

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
		User(const std::string n, const std::string u, const std::string r)
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
		}


		// destructor
		~User()
		{
		}


		// member functions
		void set_nick(std::string new_nick) { _nick = new_nick; }
		void set_pass(std::string new_pass) { _password = new_pass; }
		void set_channel(std::string new_chan) { _channel = new_chan; }
		void set_endpoint(boost::asio::ip::tcp::endpoint new_e)
		{
			endpt = new_e;
		}

		std::string get_nick() { return _nick; 	   }
		std::string get_user() { return _user_name; }
		std::string get_real() { return _real_name; }
		std::string get_pass() { return _password;  }
		std::string get_chan() { return _channel;   }
		boost::asio::ip::tcp::endpoint get_endpt() { return endpt; }

	private:
		std::string _nick;
		std::string _user_name;
		std::string _real_name;
		std::string _password;
		std::string _channel;
		boost::asio::ip::tcp::endpoint endpt; // note: remote endpoint.
};

#endif
