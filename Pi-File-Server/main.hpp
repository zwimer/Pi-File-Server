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


//Longest file name supported
#define FILE_NAME_MAX_LEN 63

//Longest command supporder
#define COMMAND_MAX_LEN 63

//Forward declarations
class SafeFile;

//For simplicity
typedef unsigned int uint;
typedef std::vector<char> data;

//Get 'name' of thread / process
std::string me(const std::string s = "");

//Common functions
int min(int a, int b);

//Used for error checking
void * safeMalloc(int s);
void Err(const char * s);
void Assert(bool b, const char * s);

#endif
