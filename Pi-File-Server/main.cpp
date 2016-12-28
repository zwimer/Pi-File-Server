#include "FileHandler.hpp"
#include "Server.hpp"

#include <iostream>
#include <signal.h>
#include <map>

//For readability
using namespace Synchronized;

//Common functions
int min(int a, int b) { 
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
void * safeFork() {
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
			sstr ss2; ss2 << "[ " << getpid()
				<< " - " << numThreads[(long)getpid()] << " ] ";
			mem[ss.str()] = ss2.str();
		}

		//Increment number of threads
		numThreads[(long)getpid()]++;	
	}

	//Return the id
	return mem[ss.str()];
}

//Main
int main(int argc, const char * argv[]) {

	//Check usage
	Assert(argc == 2, "Usage: ./a.out <port>");
	for(int i = 0; argv[1][i]; i++)
		Assert(isdigit(argv[1][i]), "Usage: ./a.out <port>");

	//Settings
	me("Master");
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

    //Start up the file syncronizing thread
    pthread_t t; pthread_create( &t, NULL, handleFileRequests, NULL );
    
    //Note that the server is now up
	log(std::string("Master server started; listening on port: " + std::to_string(port)));

	//Parent process, loop forever. Child, break on creation
	for(int v = 1; v; v = safeFork()) {

		//Wait for new connections
        sock = accept( sd, (struct sockaddr *)&client, (socklen_t*)&cLen );
		Assert(sock != -1, "sock() failed.");

		//Log the new connection
		sstr s; s << "Received incoming connection from: ";
		s << inet_ntoa( (struct in_addr)client.sin_addr ); log(s.str());
    }

	//Child: Start the server, exit when server quits
	Server a(sock); return EXIT_SUCCESS;
}
