#ifndef FILE_HANDLER_HPP
#define FILE_HANDLER_HPP

#include "PipePacket.hpp"

#include <map>


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
    
	//------------------Blocking functions------------------

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
};

#endif
