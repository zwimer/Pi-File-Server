#include "PipePacket.hpp"

#include <string.h>

//PipePacket Constructor
PipePacket::PipePacket(PP_Type typ, const char * w, const char * f, SafeFile * s)
: type(typ), file(s) { strncpy(who, w, WHO_ARR_SIZE); strncpy(name, f, FILE_ARR_SIZE); }

//Getters
const char * PipePacket::getWho() const { return who; }
const char * PipePacket::getName() const { return name; }

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

