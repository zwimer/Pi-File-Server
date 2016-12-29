#ifndef FILE_HANDLER_HPP
#define FILE_HANDLER_HPP

#include "main.hpp"

//A static class that handles file IO
class FileHandler {
public:

	//Prevent instantiation
	FileHandler() = delete;	
	
	//-------------------------Setup------------------------

	//These functions create and destroy shared memory
	//to set up this class, and prevent shared leaks respectively

	//This scans for files that exist, notes which 
	//exist, and creates any necessary files that don't.
	static void setup();

	//This deletes all shared memory
	static void destroy();
    
	//------------------Blocking functions------------------

	//Get permission to use a file
	static void getWriteAccess(const std::string& s);
	static void getReadAccess(const std::string& s);

	//Relinquish access
	static void finishReading(const std::string& s);
	static void finishWriting(const std::string& s);

	//--------------Blocking wrapper functions--------------

	//Read data from a file
	static const data read(const std::string& s);

	//Write data to a file
	static void write(const std::string& s, const data& d);
	static void write(const std::string& s, const std::string d);

	//-------------------Const variables-------------------

	//How to name interprocess items
	static const std::string filePrefix;
	static const std::string wMutexPrefix;
	static const std::string editMutexPrefix;

	//Required server files
	static const std::string logFile;
	static const std::string fileList;
	static const std::string userList;

private:

	//-----------------Non-const variables-----------------

	//Needed for shared memory
	static ShmemAllocator * allocIntSet;
	static boost::interprocess::managed_shared_memory * segment;
};

#endif
