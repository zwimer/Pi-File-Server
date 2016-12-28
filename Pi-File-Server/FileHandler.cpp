#include "FileHandler.hpp"

#include <fstream>

//For ease of reading
using namespace Synchronized;

//Define needed server files
const std::string FileHandler::logFile = "logFile";
const std::string FileHandler::infoFile = "infoFile";
const std::string FileHandler::userFile = "userFile";


//-----------------------------------Setup----------------------------------

void FileHandler::getWriteAccess();
void FileHandler::getReadAccess();

void FileHandler::finishReading();
void FileHandler::finishWriting();

//----------------------------Blocking functions----------------------------



//------------------------Blocking wrapper functions------------------------


//Read data from a file
const data FileHandler::read(const std::string& s) {

	//Get read access
	getReadAccess(s);

	//Read data in
	std::ifstream inFile( s, std::ios::binary );
	data ret( (std::istreambuf_iterator<char>(inFile)), 
              (std::istreambuf_iterator<char>()) );
	inFile.close();

	//Relinquish access
	finishReading(s);
	
	//Return data
	return ret;	
}


//Write data to a file
inline void writeFn(const std::string& s, const char * d, int n) {

	//Get write access
	getWriteAccess(s);
    
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
	finishWriting(s);
}

//Public wrappers to writeFn
void FileHandler::write(const std::string& s, const data& d) {
	writeFn( s, (char*) &d[0], (int)d.size() );
}
void FileHandler::write(const std::string& s, const std::string d) {
	writeFn( s, (char*) &d[0], (int)d.size() );
}

