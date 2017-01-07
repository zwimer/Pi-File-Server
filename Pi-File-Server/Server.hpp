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

	//For accessing thePath
	static std::string getPath();
	static void setPath(const std::string& s);

private:

	//The socket this server will use
	const int sock;

	//The currnt 'path'
	static std::mutex m;
	static std::string thePath;

	//Static constants for this class
	static const int BUFFER_SIZE;
};

#endif
