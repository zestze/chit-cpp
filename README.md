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
- c++ asio stand-alone library
	- on ubuntu this is `libasio-dev`

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

Here are instructions for doing a local session
```bash
# server side, starting at project-root
cd test/
./server-test.sh
```

```bash
# client side, starting at project-root
cd test/
./client-local-test.sh
```

If the hardcoded digital oceans server is running, then a client can sign on like so
```bash
cd test/
./client-digocean-test.sh
```

## Features to Implement

- [ ] move consts_globs etc header extern definitions into it's own definition. Globals like that
	are a bad idea
- [ ] move atomic killself to private variable of server
- [ ] Check server.h and sockio.cpp for curr changes to implement
- [ ] "Modes" for users that limit actions
- [ ] Maintain state: user profiles, chat history log, etc.
- [ ] Implement time-out mechanism on read, to account for broken connections
- [ ] Handle broken pipe exceptions, and implement a prioritize PART messages
- [ ] On client side, use different colors for different users?
- [ ] Switch from using current posix signal handler, to generic asio signal_set
	- this involves doing `io_service.run()` after signal handler is binded
