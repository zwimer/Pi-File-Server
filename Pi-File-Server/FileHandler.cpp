#include "FileHandler.hpp"
#include "SafeFile.hpp"

#include <fstream>

#ifndef NO_DEBUG
#include <iostream>
#endif

//For ease of reading
using namespace Synchronized;

//Define needed server files
const std::string FileHandler::logFile = "logFile";
const std::string FileHandler::infoFile = "infoFile";
const std::string FileHandler::userFile = "userFile";

//Create static variables 
int FileHandler::pipe[2];
bool FileHandler::setup = true;
std::map<const std::string, SafeFile*> FileHandler::fileList;


//-----------------------------------Setup----------------------------------


//The Constructor. This is the only non-static item. 
//It only exists to force initalization This function
//scans for files that exist, notes which exist, 
//and creates any necessary files that don't.
FileHandler::FileHandler(const int masterPipe[], const int internalPipe[]) {

    //Initalize pipe
    pipe[1] = masterPipe[1];    
    pipe[0] = internalPipe[0];

	//Create required server files
	fileList[logFile] = new SafeFile(logFile);
	fileList[infoFile] = new SafeFile(infoFile);
	fileList[userFile] = new SafeFile(userFile);
}


//------------------------------Called by child------------------------------


//Define the pipe to the parent
void FileHandler::setParent(int n[]) {
	pipe[0] = n[0]; pipe[1] = n[1];
}


//---------------Blocking functions called by child processes---------------


//Get permision to read or write to a file. This function makes a request to get
//access to the file, verifies the result, then returns pointer to the SafeFile when granted
SafeFile * FileHandler::getAccess(const PP_Type ping, const PP_Type pong, const std::string& s) {
    sstr s2; s2 << "getAccess failed for " << s;
	PipePacket p(ping, me().c_str(), s.c_str());
    p.sendRecvVerify(pipe, pong, s2.str().c_str());
    return p.file;
}

//Read data from a file
const data FileHandler::read(const std::string& s) {

	//Get access to the file
    SafeFile * file = getAccess(READ_REQUEST, READ_ACCESS_GRANTED, s);

	//Read data in
	std::ifstream inFile( s, std::ios::binary );
	data ret( (std::istreambuf_iterator<char>(inFile)), 
              (std::istreambuf_iterator<char>()) );
	inFile.close();

	//Relinquish access
	file->finishReading(me());
	
	//Return data
	return ret;	
}


//Write data to a file
inline void writeFn(const std::string& s, const char * d, int n) {

    //Get access to the file
    SafeFile * file = FileHandler::getAccess(WRITE_REQUEST, WRITE_ACCESS_GRANTED, s);
    
    //Append data to the file
#ifndef NO_DEBUG
    std::string s2 = "/Users/zwimer/Desktop/"+s;
    std::ofstream outFile( s2, std::ios::binary | std::ios::app );
#else
    std::ofstream outFile( s, std::ios::binary | std::ios::app );
#endif
    outFile.write( d , n*sizeof(char) );
	outFile.close();

	//Relinquish access
	file->finishWriting(me());
}

//Public wrappers to writeFn
void FileHandler::write(const std::string& s, const data& d) {
	writeFn( s, (char*) &d[0], (int)d.size() );
}
void FileHandler::write(const std::string& s, const std::string d) {
	writeFn( s, (char*) &d[0], (int)d.size() );
}

