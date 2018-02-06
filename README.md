# Chit Client and Server

## Summary

Chit is a simple chat service, written in C++11 that is based on a super simplified version of IRC.
As of now, it creates chat rooms on demand, and allows for indeterminate amount of users. The chat
options are simple, users can enter chat rooms (which will be created if one doesn't exist already),
broadcast messages across the chat room, and exit the chat room.

Channels hold topics, users, and allow channel-wide discussion. Users interact with the application
through the command-line, as a GUI will be implemented in the future, and Chit has an emphasis on
being lightweight. 

Users can choose names to display in the chat, and should perceive minimal delay when joining
channels.

As of now, no state is maintained across sessions, but this is a feature that is in development
through the use of a SQL server.

This application is written from the ground-up, only borrowing Boost's ASIO socket library, which
will be replaced in time.

## Requirements

- g++ compiler capable of running atleast C++11
- make for Makefiles
- c++ boost libraries, as of now assumes it's installed in $PATH
	- in future, will port over relevant boost libraries into `libs/` or implement own library

## Usage

First, start up a server by running `./server-test.sh` in the `test/` directory. This will by
default start a server to listen on port 8081.

Following this, users can join by running one of the other two client scripts in the `test/`
directory. One connects to a Digital Oceans server, and the other is meant for local connections.
The programs can be run with any IP address, but these scripts weren't made with that kind of
use in mind (yet).

Following this, on the client-side the program instructs the user how to connect and queries
them for relevant information, like nicknames, realnames, etc. and describes the various
commands. To quit the client-side please follow the instructions and type `EXIT`. To quit
the server side, similarly follow the output and type `CTRL+C`

## Features to Implement

- [x] Put server methods into a class, and organize a bit.
- [ ] Move 'using' declarations into class namespaces
- [ ] move consts_globs etc header extern definitions into it's own definition. Globals like that
	are a bad idea
- [ ] move atomic killself to private variable of server
- [ ] Make a notifier class that wraps a map, with [@key: channel_name, @value: pointer to
	atomic<bool>]. For each channel, use 'new' to make an atomic bool on the heap, and
	store a pointer to it. When starting the channel servlet, pass the [@value: pointer]
	and use the pointer to communicate if a user was added or not.
- [x] Get rid of globals - make a map of [cahannel_name, deque_of_stuff_
	to_pass] and make it global to main() function.
	Pass a ptr to each thread as their created, and a copy of the
	mutex protecting the map. Or a pointer of the mutex?
	Can have an atomic<bool> for each [key, value] pair to pass as well.
	Check val after grabbing lock, if not changed, return immediately,
	else, parse map and grab relevant [key, val] pair?
	Kind of unecessary, just check if val is empty.
- [x] Put extern globals in their own namespace or something.
- [ ] For errors print to std::cerr and for other info print to std::cout
- [ ] Can use structured bindings, "auto [el1, el2... ] = get_tuple(...)" when grabbing tuples
- [ ] Grab elements from tuple by calling get<type> rather than get<indx>
- [ ] Change unique_lock to scoped_lock (C++17) or lock_guard
- [x] Call "ios_base::sync_with_stdio(falste);" in functions using I/O to reduce overhead
- [ ] Check server.h and sockio.cpp for curr changes to implement
- [ ] "Modes" for users that limit actions
- [ ] Maintain state: user profiles, chat history log, etc.
- [ ] Implement time-out mechanism on read, to account for broken connections
- [ ] Handle broken pipe exceptions, and implement a prioritize PART messages
- [ ] Use RW Locks for quicker access to global variables, maybe user atomic checkVars
- [ ] On client side, use different colors for different users?
