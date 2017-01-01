#include "FileHandler.hpp"
#include "Server.hpp"

#include <iostream>
#include <signal.h>
#include <map>


//-----------------------Defined in main.hpp-----------------------


//Min function
int min(const int a, const int b) { 
	return a < b ? a : b;
}

//A function used if an assert fails
void Err(const char *s) {
	std::cout << "Error " << s << std::endl;
	perror("Perror"); exit(ERR_EXIT_CODE);
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
	sstr ss; ss << getpid() << " " << pthread_self();

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


//------------------------Only for main.cpp------------------------


//Prevent shared memory leaks
void preventSharedLeaks( int sig = 0 ) {

	//Local variables
	static pid_t pid = 0;
	int retCode = ERR_EXIT_CODE;	

	//If sig (pid > 0 for safety), kill child
	if (sig && pid > 0) kill(pid, SIGKILL);
	else if (sig) Err("pid <= 0. THIS SHOULD NEVER HAPPEN!!!");
	
	//Otherwise
	else {

		//Fork, child return
		pid = safeFork();
		if (!pid) return;

		//Parent: redirect sig-int to this function
		signal(SIGINT, preventSharedLeaks);

		//Wait for server to die
		waitpid(pid, &retCode, 0);
	}

	//Cleanup
	FileHandler::destroy();

	//Perror if needed
	if (sig && sig != SIGINT && retCode != ERR_EXIT_CODE) perror("");
	
	//Exit with the proper code
	if (sig != 0 && sig != SIGINT) exit(sig);
	exit(retCode);
}


//Main function
int main(int argc, const char * argv[]) {

	//Check usage
	Assert(argc == 2, "Usage: ./a.out <port>");
	for(int i = 0; argv[1][i]; i++)
		Assert(isdigit(argv[1][i]), "Usage: ./a.out <port>");

	//Setup
	preventSharedLeaks();
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

	//Setup the FileHandler and note that server has started up 
	sstr s; s << "Master server started; listening on port: " << port; 
	FileHandler::setup(); FileHandler::log(s);

	//Parent process: loop, Child: break
	for(int i = 1; i; i = safeFork()) {

		//Wait for new connections
		sock = accept( sd, (struct sockaddr *)&client, (socklen_t*)&cLen );
		Assert(sock != -1, "sock() failed.");

		//Log the new connection
		s.str("Received incoming connection from: ");
		s << inet_ntoa( (struct in_addr)client.sin_addr ); 
		FileHandler::log(s);
	}

	//Register this child as a new user
	FileHandler::addUser(me());

	//Child: Start the server, exit when server quits
	Server a(sock); return EXIT_SUCCESS;
}
