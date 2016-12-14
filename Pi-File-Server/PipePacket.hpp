#ifndef PIPE_PACKET_HPP
#define PIPE_PACKET_HPP

#include "main.hpp"

//Different types of PipePackets
typedef enum __PP_Type { 
	READ_REQUEST, WRITE_REQUEST, FINISH_READ, 
	FINISH_WRITE, READ_ACCESS_GRANTED,
	WRITE_ACCESS_GRANTED, ERROR_FILE_DNE
} PP_Type;

//Meant to ensure const-ness of arrays and to prevent redirection of arrays 
class fileArr { public: fileArr(const char *); char file[size] static const int size; }
class whoArr { public: whoArr(const char *); char who[size]; static const int size; }

//What will be sent through the pipes
class PipePacket {
public: 

	//Constructor
	PipePacket() = delete;
	PipePacket(PP_Type typ, const char * w, const char * n);

	//Representation
	const PP_Type type;
	const fileArr file;
	const whoArr who;
};

#endif
