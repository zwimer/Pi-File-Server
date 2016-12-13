#include "FileHandler.hpp"
#include "main.hpp"

#include <errno.h>
#include <iostream>

//For clarity
using namespace Synchronized;


//Malloc, but check to see if failed
void * safeMalloc(int s) {
	void * ret = malloc((size_t)s);
	Assert(ret, "malloc failed");
	return ret;
}

//A function used if an assert fails
void Err(const char *s) {
    std::cout << "Error at t = " << t.getTime() << ": " << s << std::endl;
    perror("Perror");
    exit(EXIT_FAILURE);
}

//A function used to test assertions
void Assert(bool b, const char *s) { if (!b) Err(s); }


//Create a string containing both
//the PID and TID of this process and thread
std::string me() {
	std::stringstream ss;
	ss << "[ Process: " << getpid() << ", thread: ";
	ss << (unsigned int) pthread_self() << " ] ";
	return std::move(ss.str());
}


//Clean up all zombies
void * cleanZombies(void*) {
	for(int i;;) while (wait(&i) == -1 && errno == EINTR);
}

//Main
int main(int argc, const char * argv[]) {

	//Start up
	pthread_t tid;
	FileHandler();
	pthread_create(&tid, NULL, cleanZombies, NULL);
	log("Server started up")
	
}
