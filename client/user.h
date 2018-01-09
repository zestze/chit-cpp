/*
 * user.h
 *
 * Zeke Reyna
 */
#ifndef __USER_H__
#define __USER_H__

#include <string>

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

		// data members
		std::string nick;
		std::string user_name;
		std::string real_name;
		std::string password;
		std::string channel;

		// member functions
		//void set_nick(std::string new_nick);
		//void set_pass(std::string new_nick);
		//void set_channel(std::string new_chan);
};

#endif
