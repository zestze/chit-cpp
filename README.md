# Chit Client and Server

## Summary

Chit is a simple chat service, written in C++11 that is based on a super simplified version of IRC.
As of now, it creates chat rooms on demand, and allows for indeterminate amount of users. The chat
options are simple, users can enter chat rooms (which will be created if one doesn't exist already),
broadcast messages across the chat room, and exit the chat room.

## Requirements

- g++ compiler capable of running atleast C++11
- make
- c++ boost libraries, as of now assumes it's installed in $PATH

## Features to Implement

- [ ] For errors print to std::cerr and for other info print to std::cout
- [ ] Can use structured bindings, "auto [el1, el2... ] = get_tuple(...)" when grabbing tuples
- [ ] Grab elements from tuple by calling get<type> rather than get<indx>
- [ ] Change unique_lock to scoped_lock (C++17) or lock_guard
- [ ] Call "ios_base::sync_with_stdio(falste);" in functions using I/O to reduce overhead
- [ ] Check server.h and sockio.cpp for curr changes to implement
- [ ] "Modes" for users that limit actions
- [ ] Maintain state: user profiles, chat history log, etc.
- [ ] Implement time-out mechanism on read, to account for broken connections
- [ ] Handle broken pipe exceptions, and implement a prioritize PART messages
- [ ] Use RW Locks for quicker access to global variables, maybe user atomic checkVars
- [ ] On client side, use different colors for different users?
