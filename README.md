# Chit Client and Server

Summary
-------
Chit is a simple chat service, written in C++11 that is based on a super simplified version of IRC.
As of now, it creates chat rooms on demand, and allows for indeterminate amount of users. The chat
options are simple, users can enter chat rooms (which will be created if one doesn't exist already),
broadcast messages across the chat room, and exit the chat room.

Features to Implement
---------------------
- [ ] "Modes" for users that limit actions
- [ ] Maintain state: user profiles, chat history log, etc.
- [ ] Implement time-out mechanism on read, to account for broken connections
- [ ] Handle broken pipe exceptions, and implement a prioritize PART messages
- [ ] Use RW Locks for quicker access to global variables, maybe user atomic checkVars
- [ ] On client side, use different colors for different users?
