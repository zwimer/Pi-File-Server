#ifndef SAFE_FILE_HPP
#define SAFE_FILE_HPP

#include "main.hpp"

#include <string>
#include <set>

//A data race free file
class SafeFile {

	//Constructor
	//Who has write access to start
	SafeFile() = delete;
	SafeFile(const char * n, const std::string& who);

	//Destructor
	~SafeFile();

	//------------Synchronized------------

	//Get permission to access, these functions block
	void getReadAccess(const std::string& who);
	void getWriteAccess(const std::string& who);

	//Relinquish rights to this file
	void finishReading(const std::string& who);
	void finishWriting(const std::string& who);
	
private:

	//Representation
	const char * name;

	//For synchronization
	std::set<std::string> users;
	pthread_mutex_t writing;
	pthread_mutex_t whoM;

};

#endif
