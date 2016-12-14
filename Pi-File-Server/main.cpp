#include "FileHandler.hpp"
#include "Server.hpp"

#include <iostream>
#include <signal.h>
#include <map>

//For clarity
using namespace Synchronized;


//Common functions
int min(int a, int b) { 
	return a < b ? a : b;
}

//Malloc, but check to see if failed
void * safeMalloc(int s) {
	void * ret = malloc((size_t)s);
	Assert(ret, "malloc failed");
	return ret;
}

//A function used if an assert fails
void Err(const char *s) {
    std::cout << "Error " << s << std::endl;
    perror("Perror: "); exit(EXIT_FAILURE);
}

//A function used to test assertions
void Assert(bool b, const char *s) { if (!b) Err(s); }


//Create a string containing both
//the PID and TID of this process and thread
//If a non-empty argument is given, remember that
//this thread should return s next time me is called
//Each thread may only call me with an argument once!
std::string me(const std::string s) {

	//Memory
	static std::map<std::string, std::string> mem;

	//If an argument was passed, remeber what was requested
	if (s != "") mem[me()] = std::string("[ ") + s + std::string(" ] ");

	//Create me
	std::stringstream ss;
	ss << "[ Process: " << getpid() << ", thread: ";
	ss << pthread_self() << " ] ";

	//Return the name of this thread if there is one
	if (mem.find(ss.str()) != mem.end()) return mem[ss.str()];

	//Otherwise just return this thread's me
	return std::move(ss.str());
}


//Main
int main(int argc, const char * argv[]) {

	//Check usage
	Assert(argc == 2, "Usage: ./a.out <port>");
	for(int i = 0; argv[1][i]; i++)
		Assert(isdigit(argv[1][i]), "Usage: ./a.out <port>");

	//Set settings
	me("Master");
	FileHandler();
	signal(SIGCHLD, SIG_IGN);

	//Local variables
    unsigned short port = atoi(argv[1]);
	struct sockaddr_in svr, client;
    int sd, cLen = sizeof(client);

    //Define the server
    svr.sin_family = PF_INET;
    svr.sin_port = htons( port );
    svr.sin_addr.s_addr = INADDR_ANY;

    //Create the listener socket as TCP, bind it, limit to 5 connections
    Assert((sd = socket( PF_INET, SOCK_STREAM, 0 )) >= 0, "socket() failed");
    Assert(bind(sd,(struct sockaddr*)&svr,sizeof(svr))>= 0, "bind() failed");
    listen( sd, 5 ); 

    //Listen to 5 connections and note so
	log(std::string("Master server started; listening on port: " + std::to_string(port)));

	//Loop
	for(;;) {

		//Wait for new connections
        int sock = accept( sd, (struct sockaddr *)&client, (socklen_t*)&cLen );

		//Log the new connection
		std::stringstream s; s << "Received incoming connection from: ";
		s << inet_ntoa( (struct in_addr)client.sin_addr ); log(s.str());

		//Create forks and threads as needed
		//Have the child start a new server
		if (smartFork() == CHILD) {
			Server(sock).start();
			exit(EXIT_SUCCESS);	
		}
    }

	//To satisfy the compiler
    return EXIT_SUCCESS;
}
