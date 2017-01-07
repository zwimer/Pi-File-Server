#ifndef MAIN_HPP
#define MAIN_HPP

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/set.hpp>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <sstream>
#include <string>
#include <vector>

//Debug mode, default on
#ifndef NO_DEBUG

//Useful for debugging
#include <iostream>
#include <libgen.h>

//A macro that prints the line, file, and function, useful in debugging
#define DBG std::cerr << basename((char*)__FILE__) << ": " << __LINE__ \
                      << " - "<< __PRETTY_FUNCTION__ << std::endl;

#endif

//Define pow2
#define pow2(P) (__pow2<P>::v)
template <int N> struct __pow2 { enum { v = 2 * pow2(N-1) }; };
template <> struct __pow2<0> { enum { v = 1 }; };

//The exit code thrown by Err
#define ERR_EXIT_CODE 3

//Define define max number of sizes
#define MAX_FILES ( pow2(24) )
#define MAX_USERS ( pow2(4) )
#define SHARE_MEM_SIZE (MAX_USERS*(4*MAX_FILES + 4*MAX_USERS + pow2(11)))

//Longest command supported
#define COMMAND_MAX_LEN 63

//Name of the master process
#define MASTER_PROC_NAME "Master"

//A simple macro that prevents the function
//it is in from running multiple times
#define RUN_ONCE { \
	static bool neverRun = true; \
	std::string s(__PRETTY_FUNCTION__); \
	s += "() should only run once!"; \
	Assert(neverRun, s.c_str()); \
	neverRun = false; \
} 

//Helpful typedef
typedef boost::interprocess::allocator<int, boost::interprocess::managed_shared_memory::segment_manager> ShmemAllocator;
typedef boost::interprocess::set<int, std::less<int>, ShmemAllocator> IntSet;

//For simplicity
typedef unsigned int uint;
typedef std::vector<char> data;
typedef std::stringstream sstr;

//Get 'name' of thread / process
std::string me(const std::string s = "");

//Common functions
int min(const int a, const int b);

//Used for error checking
void * safeMalloc(int s);
void Err(const char * s);
void Assert(bool b, const char * s);

#endif
