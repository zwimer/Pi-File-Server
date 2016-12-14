#include "FileHandler.hpp"
#include "SafeFile.hpp"

#include <fstream>

//For ease of reading
using namespace Synchronized;

//Define needed server files
const std::string FileHandler::logFile = "logFile";
const std::string FileHandler::infoFile = "infoFile";
const std::string FileHandler::userFile = "userFile";

//Create static variables 
int FileHandler::pipe[2];
std::map<const std::string, SafeFile*> FileHandler::fileList;


//-----------------------------------Setup----------------------------------


//The Constructor. This is the only non-static item. 
//It only exists to force initalization This function
//scans for files that exist, notes which exist, 
//and creates any necessary files that don't.
FileHandler::FileHandler() {

	//Initalize pipe to -1, -1
	pipe[0] = -1; pipe[1] = -1;

	//Create required server files
	fileList[logFile] = new SafeFile(logFile);
	fileList[infoFile] = new SafeFile(infoFile);
	fileList[userFile] = new SafeFile(userFile);

	log("SCANNING FOR OTHER FILES TO BE IMPLEMENTED");

}


//------------------------------Called by child------------------------------


//Define the pipe to the parent
void FileHandler::setParent(int n[]) {
	Assert(pipe[0] == -1 && pipe[1] == -1,
		"setParent() called incorrectly.");
	pipe[0] = n[0]; pipe[1] = n[1];
}


//-----------------------------Called by parent-----------------------------


//Start a new thread that listens 
//to the pipe sent through as the argument 
void * FileHandler::newChild(void * arg) {

	//Detach thread
	pthread_detach(pthread_self());

	//Local variables
	int n, size = sizeof(PipePacket);
	const int * childPipe = (int*) arg;
	PipePacket *p2, *p = (PipePacket*) safeMalloc(size);

	//While the pipe is open, take requests
	Synchronized::log("New listener thread created and waiting");
	while( ( n = ::read(childPipe[0], p, size) ) ) {

		//Check what was recieved and log it
		Assert(n==size, "Master process recieved invalid PipePacket size");
		std::stringstream ss, s2; ss << "Recieved "
;
ss  << (PipePacket)*p;
		Synchronized::log(ss.str());

		//Check usage
		if (fileList.find(p->getName()) == fileList.end() && p->type == READ_REQUEST) {

			//Create the packet to send back
			p2 = new PipePacket(ERROR_FILE_DNE, p->getWho(), p->getName());
		}

		//If valid request was sent
		else {

			//If the file doesn't exist, create it
			if (fileList.find(p->getName()) == fileList.end()) 
				fileList[p->getName()] = new SafeFile(p->getName(), p->getWho());

			//Request access, block until granted
			//Since the constructor reserves write access, this starts with else if
			else if (p->type == WRITE_REQUEST) fileList[p->getName()]->getWriteAccess(p->getWho());
			else if (p->type == READ_REQUEST) fileList[p->getName()]->getReadAccess(p->getWho());

			//Create the packet to send back
			if (p->type == WRITE_REQUEST)
				p2 = new PipePacket(WRITE_ACCESS_GRANTED, p->getWho(), p->getName());
			else if (p->type == READ_REQUEST)
				p2 = new PipePacket(READ_ACCESS_GRANTED, p->getWho(), p->getName());
		}

		//Inform child process access has been granted, and log it
		Assert( ::write(childPipe[1], p2, sizeof(PipePacket) ),
			"full PipePacket faield to send" );
		s2 << "Sent " << *p2; Synchronized::log(s2.str());
		delete p2;
	}

	//Note this child thread will now die
	Synchronized::log("This listener thread now dead");

	//Prevent leaks and return
	free(p); delete[] arg; return NULL;
}


//---------------Blocking functions called by child processes---------------


//Get permision to read or write to a file
SafeFile * FileHandler::getAccess(const PP_Type req, const PP_Type responce, const std::string& s) {

	//Make and send a PipePacket to request access
	PipePacket * p = new PipePacket(req, me().c_str(), s.c_str());
	if (::write(pipe[1], p, sizeof(PipePacket)) != sizeof(PipePacket))
		Err("full PipePacket failed to send");

	//Block until packet is recieved in return
	if (::read(pipe[0], p, sizeof(PipePacket)) != sizeof(PipePacket))
		Err("full PipePacket was not recieved");
	
	//Verify contents
	if (p->type != READ_ACCESS_GRANTED || p->getWho() != me() || p->getName() != s) {
		std::stringstream s; s << "getAccess recieved " << *p;
		Err(s.str().c_str());
	}

	//Return the SafeFile * pointer
	auto ret = p->file; delete p; return ret;
}

//Read data from a file
const data FileHandler::read(const std::string& s) {

	//Get access to the file
	auto file = getAccess(READ_REQUEST, READ_ACCESS_GRANTED, s);

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
    auto file = FileHandler::getAccess(WRITE_REQUEST, WRITE_ACCESS_GRANTED, s);

    //Append data to the file
    std::ofstream outFile( s, std::ios::binary | std::ios::app );
    outFile.write( d , n*sizeof(char) );
	outFile.close();

	//Relinquish access
	file->finishWriting(me());
}

//Public wrappers to writeFn
void FileHandler::write(const std::string& s, const data& d) {
	writeFn( s, (char*) &d[0], d.size() );
}
void FileHandler::write(const std::string& s, const std::string d) {
	writeFn( s, (char*) &d[0], d.size() );
}

