#include "FileHandler.hpp"
#include "SafeFile.hpp"

//Define needed server files
const std::string FileHandler::logFile = "logFile";
const std::string FileHandler::fileInfo = "fileInfo";
const std::string FileHandler::userList = "userList";


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
	fileList[fileInfo] = new SafeFile(fileInfo);
	fileList[userList] = new SafeFile(userList);

	log("SCANNING FOR OTHER FILES TO BE IMPLEMENTED");

}


//------------------------------Called by child------------------------------


//Define the pipe to the parent
static void FileHandler::setParent(int n[]) {
	Assert(pipe[0] == -1 && pipe[1] == -1,
		"setParent() called incorrectly.");
	pipe[0] = n[0]; pipe[1] = n[1];
}


//-----------------------------Called by parent-----------------------------


//Start a new thread that listens 
//to the pipe sent through as the argument 
static void * FileHandler::newChild(void * arg) {

	//Detach thread
	pthread_detach(pthread_self());

	//Local variables
	int n, size = sizeof(PipePacket);
	const int * childPipe = (int*) arg;
	PipePacket *p2, *p = safeMalloc(size);

	//While the pipe is open, take requests
	Synchronized::log("New listener thread created and waiting");
	while( ( n = read(childPipe[0], p, size) ) ) {

		//Check what was recieved and log it
		Assert(n==size, "Master process recieved invalid PipePacket size");
		std::stringstream ss, s2; ss << "Recieved " << *p;
		Synchronized::log(ss.str());

		//Check usage
		if (fileList.find(p->file) == fileList.end() && p->type == READ_REQUEST) {

			//Create the packet to send back
			p2 = new PipePacket(ERROR_FILE_DNE, p->who, p->file);
		}

		//If valid request was sent
		else {

			//If the file doesn't exist, create it
			if (fileList.find(p->file) == fileList.end()) 
				fileList[p->file] = new SafeFile(p->file, p->who);

			//Request access, block until granted
			//Since the constructor reserves write access, this starts with else if
			else if (p->type == WRITE_REQUEST) fileList[p->file]->getWriteAccess(p->who);
			else if (p->type == READ_REQUEST) fileList[p->file]->getReadAccess(p->who);

			//Create the packet to send back
			if (p->type == WRITE_REQUEST)
				p2 = new PipePacket(WRITE_ACCESS_GRANTED, p->who, p->file);
			else if (p->type == READ_REQUEST)
				p2 = new PipePacket(READ_ACCESS_GRANTED, p->who, p->file);
		}

		//Inform child process access has been granted, and log it
		Assert( write(childPipe[1], p2, sizeof(PipePacket) );
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
void FileHandler::getAccess(const PP_Type req, const PP_TYPE responce) {

	//Make and send a PipePacket to request access
	PipePakcet * p = new PipePacket(req, me(), s);
	if (write(pipe[1], p, sizeof(PipePacket)) != sizeof(PipePacket))
		Err("Error, full PipePacket failed to send");

	//Block until packet is recieved in return
	if (read(pipe[0], p, sizeof(PipePacket)) != sizeof(PipePacket))
		Err("Error, full PipePacket was not recieved");
	
	//Verify contents
	if (p->type != READ_ACCESS_GRANTED || p->who != me() || p->file != s) {
		std::stringstream s; s << "Error, getAccess recieved " << *p;
		Err(s.str());
	}
}

//Read data from a file
static const data FileHandler::read(const std::string& s) {

	//Get access to the file
	getAccess(READ_REQUEST, READ_ACCESS_GRANTED);

	//Read data in
	std::ifstream inFile( s, std::ios::binary );
	data ret( (std::istreambuf_iterator<char>(inFile)), 
              (std::istreambuf_iterator<char>()) );
	inFile.close();

	//Relinquish access
	safeFile.finishReading(me());
	
	//Return data
	return ret;	
}


//Write data to a file
inline void writeFn(const std::string& s, const char * d, int n) {

    //Get access to the file
    getAccess(WRITE_REQUEST, WRITE_ACCESS_GRANTED);

    //Append data to the file
    std::ofstream outFile( s, std::ios::binary | std::ios::app );
    outFile.write( d , n*sizeof(char) );
    outFile.close();

	//Relinquish access
	safeFile.finishWriting(me());
}

//Public wrappers to writeFn
static void FileHandler::write(const std::string& s, const data& d) {
	writeFn( s, (char*) &d[0], d.size() );
}
static void FileHandler::write(const std::string& s, const std::string d) {
	writeFn( s, (char*) &d[0], d.size() );
}

