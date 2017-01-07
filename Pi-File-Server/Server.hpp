#ifndef SERVER_HPP
#define SERVER_HPP

#include "main.hpp"

#include <mutex>

//A server that will handle
//all requests made on fd sock
class Server final {
public:

	//Constructor
	Server() = delete;
	Server(int s);
	
	//Destructor
	~Server();

	//Run the server
	void start();

	//Get the current path
	std::string getPath() const;

	//Change the path
	void setPath(const std::string& s);

private:

	//The currnt 'path'
	std::string thePath;
	mutable std::mutex m;

	//The socket this server will use
	const int sock;

	//Static constants for this class
	static const int BUFFER_SIZE;
};

#endif
