#ifndef FILE_HANDLER_HPP
#define FILE_HANDLER_HPP

#include "PipePacket.hpp"

#include <map>

/*
 * A quick explaination:
 *
 * The cleint connects. Main's main thread calls smart fork. The main
 * process then starts a new thread that runs newChild. Afterwards,
 * the client sends a request to 'do something', we will call this
 * act. The child thread interprets this then does whatever is
 * needed. If it requires reading from a file then read or
 * write is called. This then creates a PipePacket, which
 * is sent to the main process and recieved by the newChild
 * function. This function reads the PipePacket, and calls
 * a getAcess function to request permission to read or write
 * to the file. These block until access has been granted.
 * Once granted, newChild informs the child process that
 * they can preform act to the file. Once the read or write
 * function finishes, it will let newChild know, via a 
 * PipePacket, that the child has finished using the file,
 * and relinquishes its right to preform act on it.
 *
 */

//Forward declearations
class SafeFile;

//A static class that handles file IO
class FileHandler {
public:
	
	//-------------------------Setup------------------------

	//The Constructor. This is the only non-static item. 
	//It only exists to force initalization This function
	//scans for files that exist, notes which exist, 
	//and creates any necessary files that don't.
	FileHandler();

	//--------------------Called by child--------------------

	//Define the pipe to the parent
	static void setParent(int n[]);

	//-------------------Called by parent-------------------

	//Start a new thread that listens 
	//to the pipe sent through as the argument 
	static void * newChild( void * arg );

	//-----Blocking functions called by child processes-----

	//Requests req access, and verifies responce was returned
	static SafeFile * getAccess(PP_Type, PP_Type, const std::string&);

	//Read data from a file
	static const data read(const std::string& s);

	//Write data to a file
	static void write(const std::string& s, const data& d);
	static void write(const std::string& s, const std::string d);

	//-------------------Const variables-------------------

	//Required server files
	static const std::string logFile;
	static const std::string infoFile;
	static const std::string userFile;

private:

	//List of all safe files
	static std::map<const std::string, SafeFile*> fileList;

	//This stays the same across threads
	//But changes depending on the process
	static int pipe[2];
    
    //True before constructor has been called
    static bool setup;
};

#endif
