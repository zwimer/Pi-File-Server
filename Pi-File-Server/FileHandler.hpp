#ifndef FILE_HANDLER_HPP
#define FILE_HANDLER_HPP

#include "Server.hpp"

#include <map>

/*
This is a class that allows multiple processes to write to the same
'physical' files on the hard-drive. This is the lowest level type of
file. This system prevents data-races, but deadlock is still possible
is multiple processes are waiting for access on files each other has
access to. Preventing this is the responsibility of whatever uses this
file-sync system. 

There are three files which are required by FileHandler, listed below.

## Required Files:

### FileList
- Files added to this will be 'safe'
- For now: Nothing may EVER be removed from this

### UserList
- This is NOT a 'safe' file. To access this file, even for reading, one must acquire the write mutex of this file.
- This list keeps track of processes allowed to access this file
- For now: Nothing may EVER be removed from this

### Log file
- A record of what has happened in the program so far

 */


//A static class that handles file IO
class FileHandler {
public:

	//Prevent instantiation
	FileHandler() = delete;	
	
	//Friend functions
	friend Server::~Server();
	friend void preventSharedLeaks(int);
	
	//-------------------------Setup------------------------

	//These functions create and destroy shared memory
	//to set up this class, and prevent shared leaks respectively

	//This scans for files that exist, notes which 
	//exist, and creates any necessary files that don't.
	static void setup();

	//------------------Blocking functions------------------

	//Get permission to use a file
	static void getWriteAccess(const std::string& s);
	static void getReadAccess(const std::string& s);

	//Relinquish access
	static void finishReading(const std::string& s, std::string who = "");
	static void finishWriting(const std::string& s);

	//--------------Blocking wrapper functions--------------

	//Used to log an action
	static void log(const sstr& s);
	static void log(const char * s);
	static void log(const std::string& s);

	//Read data from a file
	static const data read(const std::string& s);

	//Write data to a file
	static void append(const std::string& s, const data& d);
	static void overWrite(const std::string& s, const data& d);
	static void append(const std::string& s, const std::string d);
	static void overWrite(const std::string& s, const std::string d);

	//Add a me to the user list
	static void addMe();

private:

	//-----------------------Clean up-----------------------

	//This deletes all shared memory
	static void destroy();

	//Remove this user's file permissions
	static void userQuit(const std::string& s);

	//--------------Blocking 'worker' functions-------------

	//These functions may not be called on userList

	//Get permission to use a file
	static void getWriteAccessP(const std::string& s);
	static void getReadAccessP(const std::string& s);

	//Relinquish access
	static void finishReadingP(const std::string& s, const std::string& who);
	static void finishWritingP(const std::string& s);

	//Write data to a file
	static void writeP( const std::string& s, const char * d, const int n,
	                    const bool safe, const bool app = true );

	//-------------------Helper functions-------------------

	//Returns false if the file s shouldn't be accessible
	static bool legalFile(const std::string& s);

	//---------------Blocking helper functions--------------

	//Read and parse file
	static std::vector<std::string> * readAndParse(const std::string&, const bool);
		
	//Check if an item exists in file f (either userList or fileList)
	//If index != NULL, the index the items is at will be stored in index
	//and userList will be used instead of fileList. If getAccess, and we
	//are reading from the fileList, get read access before reading the file
	static bool itemExists( const std::string& s, const bool getAccess = true,
	                        int * index = NULL, bool usrLst = false );

	//-------------------Const variables-------------------

	//Required server files
	static const std::string logFile;
	static const std::string fileList;
	static const std::string userList;

	//How to name interprocess items
	static const std::string filePrefix;
	static const std::string userPrefix;
	static const std::string wMutexPrefix;
	static const std::string editMutexPrefix;

	//-----------------Non-Const variables-----------------

	//Needed for shared memory
	static ShmemAllocator * allocIntSet;
	static boost::interprocess::managed_shared_memory * segment;
};

#endif
