/*
 * consts_globs_shared.h
 *
 * Holds all the global constants, structures, and functions
 * that should be included in server.h and servlet.h
 *
 * Zeke Reyna
 */
#ifndef __CONSTS_GLOBS_SHARED_H__
#define __CONSTS_GLOBS_SHARED_H__

#include <string>
#include <atomic>

// *************** constants ************
const std::string RPL_WELCOME = "001";
const std::string RPL_TOPIC = "332";
const std::string RPL_NAMREPLY = "252";
const std::string RPL_ENDOFNAMES = "366";

// *************** shared ****************
// for purpose of telling threads to exit
extern std::atomic<bool> killself;

#endif
