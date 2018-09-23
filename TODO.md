# TODO.md

A more centralized version of all the TODOs scattered around the program.

Build a google Test infrastructure for chitter and such.

Figure out a way to get rid of chitter stuff on client-side so a user can easily
compile the client side with little fuss

Make a dbConnectionPool class that's implemented as a queue and gives a server
threaded access to the database so it doesn't have to wait on the same connection
to register things with the database.