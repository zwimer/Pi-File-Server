#ifndef MAIN_HPP
#define MAIN_HPP


//-------------------------Includes------------------------


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
#include <mutex>


//-----------------------Template meta---------------------


//Define pow2
template <int N> struct __pow2 { enum { v = 2 * __pow2<N-1>::v }; };
template <> struct __pow2<0> { enum { v = 1 }; };


//Check if Sub is a subclass of Sup
template <class Sub, class Sup> class __SubClass {
	
	//Declare two type of different sizes
	typedef char Small; class Big { char a[2]; };

	//Test returns a Small if Sub is a
	//subclass of Sup. Otherwise, Test returns true
	static Small Test(const Sup&); static Big Test(...);

	//A function which returns a sub, used so
	//that below a Sub need not be constructed
	static Sub SubvertConstruction();

public:

	//Because sizeof doesn't evaluate anything, this doesn't need to create
	//any objects. SubvertConstruction returns a Sub. Thus Test takes in a Sub.
	//The first definition of Test takes in a Sup; if this definition is accepted,
	//then Sub is a subclass of Sup. If so, Test returns a Small. valid will thus
	//be true as sizeof(Small) == sizeof(Small). However, if Sub is not a subclass
	//of sup, then Test(...) will be used, which will return a Big. Thus valid = false
	enum { valid = sizeof(Test(SubvertConstruction())) == sizeof(Small) };
};

//False if the two types are the same
template <class T> class __SubClass<T, T> { public: enum { valid = 0 }; };


//---------------------------Macros-------------------------


//Template-meta wrappers
#define pow2(P) (__pow2<P>::v)
#define SubClass(A,B) __SubClass<A,B>::valid

//Debug mode, default on
#ifndef NO_DEBUG

	//Useful for debugging
	#include <iostream>
	#include <libgen.h>

	//A macro that prints the line, file, and function, useful in debugging
	#define DBG std::cerr << basename((char*)__FILE__) << ": " << __LINE__ \
						  << " - "<< __PRETTY_FUNCTION__ << std::endl;

#endif

//Prevents something from running multiple times
//Is thread-safe, but should by principle only be
//used by functions called before threading occurs
#define RUN_ONCE {                      \
	static bool neverRun = true;        \
	static std::mutex m; m.lock();      \
	std::string s(__PRETTY_FUNCTION__); \
	s += "() should only run once!";    \
	Assert(neverRun, s.c_str());        \
	neverRun = false;                   \
	m.unlock();                         \
} 


//-------------------------Constants-----------------------


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


//--------------------------Typedefs------------------------


//Helpful typedef
typedef boost::interprocess::allocator
		< int, boost::interprocess::managed_shared_memory::segment_manager >
		ShmemAllocator;
typedef boost::interprocess::set< int, std::less<int>, ShmemAllocator > IntSet;

//For simplicity
typedef unsigned int uint;
typedef std::vector<char> data;
typedef std::stringstream sstr;


//--------------------Function Prototypes------------------


//Get 'name' of thread / process
std::string me(const std::string s = "");

//Common functions
int min(const int a, const int b);

//Used for error checking
void Err(const char * s);
void Assert(bool b, const char * s);


#endif
