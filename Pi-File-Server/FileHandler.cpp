#include "Synchronized.hpp"


//Initalize pipe to -1, -1
FileHandler::pipe[0] = -1;
FileHandler::pipe[1] = -1;

//Define types of PipePackets
const int PipePacket::READ = 0; 
const int PipePacket::WRITE = 1; 
const int PipePacket::FINISH_READ = 2; 
const int PipePacket::FINISH_WRITE = 3; 

//Define needed server files
const std::string FileHandler::logFile = "logFile";
const std::string FileHandler::fileList = "fileList";
const std::string FileHandler::userList = "userList";


//PipePacket Constructor
PipePacket(int typ, const char * w, const char * n)
 : type(typ), who(w), file(n) {}

//Overload the stream operator
std::ostream& operator << (std::ostream& s, const PipePacket& p) {
	std::string tmp = p.who; tmp.pop_back();
	s << "PipePacket( " << (p.type==PipePacket::READ?"Read":"Write");
	s << ", " << tmp << ", " << p.file << " )";
}

//-----------------------------Setup----------------------------


//The Constructor. This is the only non-static item. 
//It only exists to force initalization This function
//scans for files that exist, notes which exist, 
//and creates any necessary files that don't.
FileHandler();

//Child: Define the pipe to write to
static void FileHandler::setParent(int n[]) {
	Assert(pipe[0] == -1 && pipe[1] == -1,
		"setParent() called incorrectly.");
	pipe[0] = n[0]; pipe[1] = n[1];
}

//Parent: Starts a new thread whose job
//it is to listen to the child process
static void * FileHandler::newChild(void * n) {

	//Detach thread
	pthread_detach(pthread_self());

	//Local variables
	int n, size = sizeof(PipePacket);
	PipePacket * p = safeMalloc(size);

	//While the pipe is open, take requests
	Synchronized::log("New listener thread created and waiting");
	while( ( n = read(pipe[0], p, size) ) ) {

		//Check what was recieved and log it
		Assert(n==size, "Master process recieved invalid PipePacket size");
		std::stringstream ss; ss << "Recieved " << *p;
		Synchronized::log(ss.str());

		//Request access, block until granted
		if (p->type == PipePacket::WRITE) getWriteAccess(p->file);
		else if (p->type == PipePacket::READ) getReadAccess(p->file);

		//Create messages
		std::stringstream s2, s3;
		std::string msg = "Access Granted";
		s2 << ME() << msg; s3 << "Sent " << msg;
		std::string s = s2.str();

		//Inform child process access has been granted, and log it
		Assert( write(pipe[1], s.c_str(), s.size()) == s.size() );
		Synchronized::log(s3.str());
	}

	//Note this child thread will now die
	Synchronized::log("This listener thread now dead");

	//Prevent leaks
	free(p); return NULL;
}


//-----Blocking functions called by child processes-----


//Read data from a file
static const data FileHandler::read(const std::string& s);

//Write data to a file
static void FileHandler::write(const std::string& s, const data& d);
static void FileHandler::write(const std::string& s, const std::string d);

