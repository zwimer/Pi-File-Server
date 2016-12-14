#include "PipePacket.hpp"

//Define constants
int whoArr::size = 64;
int fileArr::size = FILE_NAME_MAX_LEN + 1;

//Meant to ensure const-ness of arrays and to prevent redirection of arrays 
fileArr::fileArr(const char * f) {
	strncpy(file, f, size-1); file[size-1] = 0;
}
whoArr::whoArr(const char * w) {
	strncpy(who, w, size-1); who[size-1] = 0;
}

//PipePacket Constructor
PipePacket::PipePacket(PP_Type typ, const char * w, const char * f)
 : type(typ), file(f), who(w) {}

//Overload the stream operator
std::ostream& operator << (std::ostream& s, const PipePacket& p) {

	//Determine create pieces of string to add
	std::string tmp2 = p.who; tmp2.pop_back(),	tmp = "Other";
	if (p.type == READ_REQUEST)					tmp = "Read request";
	else if (p.type == WRITE_REQUEST)			tmp = "Write request";
	else if (p.type == FINISH_READ)				tmp = "Finish read";
	else if (p.type == FINISH_WRITE)			tmp = "Finish write";
	else if (p.type == READ_ACCESS_GRANTED)		tmp = "Read Access Granted";
	else if (p.type == WRITE_ACCESS_GRANTED)	tmp = "Write Access Granted";
	else if (p.type == ERROR_FILE_DNE)			tmp = "Error File DNE";

	//Write to the stream and return it
	s << "PipePacket( " << tmp << ", ";
	s << tmp2 << ", " << p.file << " )";
	return s;
}


