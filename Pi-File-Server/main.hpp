#ifndef MAIN_HPP
#define MAIN_HPP

#include "Synchronized.hpp"

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

//Define pow2
#define pow2(P) (__pow2<P>::v)
template <int N> struct __pow2  { enum { v = 2 * pow2(N-1) }; };
template <> struct __pow2<0> { enum { v = 1 }; };

//Define define max number of sizes
#define MAX_FILES ( pow2(24) )
#define MAX_USERS ( pow2(4) )
#define SHAR_MEM_SIZE (MAX_USERS*(MAX_FILE + 1024))

//Longest command supporder
#define COMMAND_MAX_LEN 63

//Forward declarations
class SafeFile;

//For simplicity
typedef unsigned int uint;
typedef std::vector<char> data;
typedef std::stringstream sstr;

//Get 'name' of thread / process
std::string me(const std::string s = "");

//Common functions
int min(int a, int b);

//Used for error checking
void * safeMalloc(int s);
void Err(const char * s);
void Assert(bool b, const char * s);

#endif
