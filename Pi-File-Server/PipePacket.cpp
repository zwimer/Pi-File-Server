#include "PipePacket.hpp"

#include <string.h>

//PipePacket Constructors
PipePacket::PipePacket(const PipePacket * p) : type(p->type), file(p->file),
ProcessNum(p->ProcessNum) {
    strncpy(name, p->getName(), FILE_ARR_SIZE); name[FILE_ARR_SIZE] = 0;
    strncpy(who, p->getWho(), WHO_ARR_SIZE); who[WHO_ARR_SIZE] = 0;
}

//Normal constructor
PipePacket::PipePacket(PP_Type typ, const char * w, const char * f, SafeFile * s)
: type(typ), file(s), ProcessNum(getpid()) {
    strncpy(name, f, FILE_ARR_SIZE); name[FILE_ARR_SIZE] = 0;
    strncpy(who, w, WHO_ARR_SIZE); who[WHO_ARR_SIZE] = 0;
}

//Getters
const char * PipePacket::getWho() const { return who; }
const char * PipePacket::getName() const { return name; }

//Write wrappers
void PipePacket::write(int fd) const {
    const static int a = sizeof(PipePacket);
    Assert(::write(fd, (void*)this, a) == a, "write() failed");
}

//Read wrapper
void PipePacket::read(int fd) {
    const static int a = sizeof(PipePacket);
    Assert(::read(fd, (void*)this, a) == a, "read() failed");
}

//Calls write, then read, then verifies the response
void PipePacket::sendRecvVerify(int p[], const PP_Type responce, const char * Msg) {
    
    //Send the packet, then get the responce
    PipePacket p2(this); p2.write(p[1]); p2.read(p[0]);
    
    //Verify
    Assert(type == responce && getWho() == p2.getWho() &&
           getName() == p2.getName() && ProcessNum == p2.ProcessNum, Msg);
}

//Overload the stream operator
std::ostream& operator << (std::ostream& s, const PipePacket& p) {

	//Determine create pieces of string to add
	std::string tmp, tmp2 = p.getWho(); tmp2.pop_back(); tmp = "Other";
	if (p.type == READ_REQUEST)					 tmp = "Read request";
	else if (p.type == WRITE_REQUEST)			 tmp = "Write request";
	else if (p.type == FINISH_READ)				 tmp = "Finish read";
	else if (p.type == FINISH_WRITE)			 tmp = "Finish write";
	else if (p.type == READ_ACCESS_GRANTED)		 tmp = "Read Access Granted";
	else if (p.type == WRITE_ACCESS_GRANTED)	 tmp = "Write Access Granted";
	else if (p.type == ERROR_FILE_DNE)			 tmp = "Error File DNE";

	//Write to the stream and return it
	s << "PipePacket( " << tmp << ", ";
	s << tmp2 << ", " << p.getName() << " )";
	return s;
}

