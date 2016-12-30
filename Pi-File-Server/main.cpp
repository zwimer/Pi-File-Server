#include "FileHandler.hpp"
#include "Server.hpp"

#include <iostream>
#include <signal.h>
#include <map>

//For readability
using namespace Synchronized;

//Common functions
int min(const int a, const int b) { 
	return a < b ? a : b;
}

//A function used if an assert fails
void Err(const char *s) {
    std::cout << "Error " << s << std::endl;
    perror("Perror"); exit(EXIT_FAILURE);
}

//A function used to test assertions
void Assert(bool b, const char *s) { if (!b) Err(s); }

//Fork, but check to see if failed
int safeFork() {
	int ret = fork();
	Assert(ret != -1, "fork() failed");
	return ret;
}

//Malloc, but check to see if failed
void * safeMalloc(int s) {
	void * ret = malloc((size_t)s);
	Assert(ret, "malloc() failed");
	return ret;
}


//Create a string unique to this thread of this process
//If an argument is provided, make that the unqique
//identifier of this thread of this process
std::string me(const std::string s) {

	//Memory
	static std::map<long, int> numThreads;
	static std::map<std::string, std::string> mem;

	//Create map key
	sstr ss; ss << getpid() << ',' << pthread_self();

	//If the process is not in the map, add it
	if (mem.find(ss.str()) == mem.end()) {

		//If an argument was passed, use this as the map value
		if (s != "") mem[ss.str()] = std::string("[ ") + s + std::string(" ] ");
	
		//Otherwise construct one
		else {
			sstr ss2; ss2 << "[ " << getpid() << " - "
				<< numThreads[(long)getpid()] << " ] ";
			mem[ss.str()] = ss2.str();
		}

		//Increment number of threads
		numThreads[(long)getpid()]++;	
	}

	//Return the id. Because of C++11 standards for
	//strings, it is safe to return this local variable
	return mem[ss.str()];
}

//Prevent shared memory leaks
void startMemoryDestroyingProc() {

	//Fork, child return
	pid_t rc = safeFork();
	if (!rc) return;

	//Parent, wait for server to die, then clean up
	int ret; waitpid(rc, &ret, 0);
	FileHandler::destroy();
	exit(ret);
}

//Main
int main(int argc, const char * argv[]) {

	//Check usage
	Assert(argc == 2, "Usage: ./a.out <port>");
	for(int i = 0; argv[1][i]; i++)
		Assert(isdigit(argv[1][i]), "Usage: ./a.out <port>");

	//Prevent shared memory leaks
	startMemoryDestroyingProc();

	//Settings
	me(MASTER_PROC_NAME);
	signal(SIGCHLD, SIG_IGN);

	//Local variables
	struct sockaddr_in svr, client;
    int sd, sock, cLen = sizeof(client);
    unsigned short port = atoi(argv[1]);

    //Define the server
    svr.sin_family = PF_INET;
    svr.sin_port = htons( port );
    svr.sin_addr.s_addr = INADDR_ANY;

    //Create the listener socket as TCP, bind it, limit to 5 connections
    Assert((sd = socket( PF_INET, SOCK_STREAM, 0 )) >= 0, "socket() failed");
    Assert(bind(sd,(struct sockaddr*)&svr,sizeof(svr))>= 0, "bind() failed");
    listen( sd, 5 );

    //Note that the master server is now up
	sstr s; s << "Master server started; listening on port: " << port; 
	FileHandler::setup(); log(s.str());

	//Parent process: loop, Child: break
	for(int i = 1; i; i = safeFork()) {

		//Wait for new connections
        sock = accept( sd, (struct sockaddr *)&client, (socklen_t*)&cLen );
		Assert(sock != -1, "sock() failed.");

		//Log the new connection
		s.str(""); s << "Received incoming connection from: ";
		s << inet_ntoa( (struct in_addr)client.sin_addr ); log(s.str());
    }

	//Register this child as a new user
	FileHandler::addUser(me());

	//Child: Start the server, exit when server quits
	Server a(sock); return EXIT_SUCCESS;
}
