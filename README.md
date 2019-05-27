# Chit Client and Server
[![Build Status](https://travis-ci.org/zestze/chit-cpp.svg?branch=master)](https://travis-ci.org/zestze/chit-cpp)

## Summary

Chit is a simple chat service, written in Modern C++ that is based on a super simplified version of IRC.
As of now, it creates chat rooms on demand, and allows for indeterminate amount of users. The chat
options are simple, users can enter chat rooms (which will be created if one doesn't exist already),
broadcast messages across the chat room, and exit the chat room.

Channels hold topics, users, and allow channel-wide discussion. Users interact with the application
through the command-line, as a GUI will be implemented in the future, and Chit has an emphasis on
being lightweight. 

Users can choose names to display in the chat, and should perceive minimal delay when joining
channels.

State is maintained across sessions through the use of a PostgreSQL server.

This application is written from the ground-up, only borrowing the ASIO socket library, and
the libpq/libpqxx libraries for interfacing with PostgreSQL.

## Requirements

- c++ compiler capable of c++17
    - development done with clang++-6.0
- make
    - development done with make-4.1
- cmake
    - development done with cmake-3.10
- libpq and libpqxx
    - c and c++ libraries, respectively, for working with PostgreSQL
- gtest and gmock
    - c++ testing suite
- asio
    - c++ socket library

## Usage

Here are instructions for doing a local session
```bash
# server side, starting at project-root
cd test/scripts/    # move to the scripts directory
./install.sh        # basically 'install' / 'build' the project's binaries
./server-test.sh    # the actual script to run to execute the server binary
```

```bash
# client side, starting at project-root
cd test/scripts/          # move to the scripts directory
./install.sh              # don't run if you already to ran to build the server
./client-local-test.sh    # the actual script to run to execute the client binary
```

If the hardcoded digital oceans server is running, then a client can sign on like so
```bash
cd test/scripts/
./install.sh
./client-digocean-test.sh
```

## Features to Implement

- [ ] "Modes" for users that limit actions
- [ ] Implement time-out mechanism on read, to account for broken connections
- [ ] Handle broken pipe exceptions, and implement a prioritize PART messages
- [ ] On client side, use different colors for different users?
- [ ] Switch from using current posix signal handler, to generic asio signal_set
	- this involves doing `io_service.run()` after signal handler is binded
- [ ] Server is still getting `bad_alloc` ocasionally - find out why.
