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
			nick      = "";
			user_name = "";
			real_name = "";
			password  = "";
			channel   = "";
		}

		// constructor with params
		User(const std::string n, const std::string u, const std::string r)
			:nick{n}, user_name{u}, real_name{r}
		{
			password = "";
			channel  = "";
		}

		// copy constructor
		User(const User& other)
		{
			nick      = other.nick;
			user_name = other.user_name;
			real_name = other.real_name;
			password  = other.password;
			channel   = other.channel;
		}


		// destructor
		~User()
		{
		}


		// member functions
		void set_nick(std::string new_nick) { nick = new_nick; }
		void set_pass(std::string new_pass) { password = new_pass; }
		void set_channel(std::string new_chan) { channel = new_chan; }
		void set_endpoint(boost::asio::ip::tcp::endpoint new_e)
		{
			endpt = new_e;
		}

		std::string get_nick() { return nick; 	   }
		std::string get_user() { return user_name; }
		std::string get_real() { return real_name; }
		std::string get_pass() { return password;  }
		std::string get_chan() { return channel;   }
		boost::asio::ip::tcp::endpoint get_endpt() { return endpt; }

	private:
		std::string nick;
		std::string user_name;
		std::string real_name;
		std::string password;
		std::string channel;
		boost::asio::ip::tcp::endpoint endpt; // note: remote endpoint.
};

#endif
