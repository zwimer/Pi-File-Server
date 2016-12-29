#ifndef MAIN_HPP
#define MAIN_HPP

#include "Synchronized.hpp"

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/set.hpp>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <sstream>
#include <vector>

//TODO
#ifndef NO_DEBUG
#include <iostream>
#endif

//Define pow2
#define pow2(P) (__pow2<P>::v)
template <int N> struct __pow2  { enum { v = 2 * pow2(N-1) }; };
template <> struct __pow2<0> { enum { v = 1 }; };

//Define define max number of sizes
#define MAX_FILES ( pow2(24) )
#define MAX_USERS ( pow2(4) )
#define SHARE_MEM_SIZE (MAX_USERS*(MAX_FILES + 1024))

//Longest command supported
#define COMMAND_MAX_LEN 63

//Name of the master process
#define MASTER_PROC_NAME "Master"

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
