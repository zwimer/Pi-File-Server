#ifndef FILE_HANDLER_HPP
#define FILE_HANDLER_HPP

#include "main.hpp"

#include <mutex>
#include <map>

/*
This is a class that allows multiple processes to write to the same
'physical' files on the hard-drive. This is the lowest level type of
file. This system prevents data-races, but deadlock is still possible
is mutliple processes are waiting for access on files each other has
access to. Preventing this is the responsibility of whatever uses this
file-sync system. 

There are three files which are required by FileHandler, listed below.

##Required Files:

###FileList
- Files added to this will be 'safe'
- For now: Nothing may EVER be removed from this

###UserList
- This is NOT a 'safe' file. To access this file, even for reading, one must aquire the write mutex of this file.
- This list keeps track of processes allowed to access this file
- For now: Nothing may EVER be removed from this

###Log file
- A record of what has happened in the program so far

 */


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

	//-----------------------Clean up-----------------------

	//This deletes all shared memory
	static void destroy();

	//Remove this user's file permissions
	static void userQuit(const std::string& s);
    
	//------------------Blocking functions------------------

	//Get permission to use a file
	static void getWriteAccess(const std::string& s);
	static void getReadAccess(const std::string& s);

	//Relinquish access
	static void finishReading(const std::string& s, std::string who = "");
	static void finishWriting(const std::string& s);

	//--------------Blocking wrapper functions--------------

	//Read data from a file
	static const data read(const std::string& s);

	//Write data to a file
	static void append(const std::string& s, const data& d);
	static void overWrite(const std::string& s, const data& d);
	static void append(const std::string& s, const std::string d);
	static void overWrite(const std::string& s, const std::string d);

	//Add a user to the user list
	static void addUser(const std::string& s);

	//-------------------Const variables-------------------

	//How to name interprocess items
	static const std::string filePrefix;
	static const std::string userPrefix;
	static const std::string wMutexPrefix;
	static const std::string editMutexPrefix;

	//Required server files
	static const std::string logFile;
	static const std::string fileList;
	static const std::string userList;

private:

	//------------------Blocking functions------------------

	//These functions may not be called on userList

	//Get permission to use a file
	static void getWriteAccessP(const std::string& s);
	static void getReadAccessP(const std::string& s);

	//Relinquish access
	static void finishReading(const std::string& s, std::string who = "");
	static void finishWriting(const std::string& s);


	//-----------------Non-const variables-----------------

	//Needed for shared memory
	static ShmemAllocator * allocIntSet;
	static boost::interprocess::managed_shared_memory * segment;
};

#endif
