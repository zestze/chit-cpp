/*
 * globals.h
 *
 * Holds all the global constants, structures, and functions
 * that should be included in server.h and servlet.h
 * for communication purposes
 *
 * Zeke Reyna
 */
#ifndef __CONSTS_GLOBS_SHARED_H__
#define __CONSTS_GLOBS_SHARED_H__

#include <atomic>

// *************** shared ****************
// for purpose of telling threads to exit
//@TODO: get rid of, instead pass pointer to atomic from server instance to its servlets.
extern std::atomic<bool> killself;

#endif
