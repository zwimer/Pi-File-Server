#ifndef SERVER_HPP
#define SERVER_HPP

#include "main.hpp"

//A server that will handle
//all requests made on fd sock
class Server final {
public:

	//Constructor
	Server() = delete;
	Server(int s);
	
	//Destructor
	~Server();

	//The function that runs the server
	void start();

private:

	//The fd this server will listen to
	const int sock;

	//Static constants for this class
	static const int BUFFER_SIZE;
};

#endif
