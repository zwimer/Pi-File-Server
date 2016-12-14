#ifndef PIPE_PACKET_HPP
#define PIPE_PACKET_HPP

#include "main.hpp"

//Define sizes
#define WHO_ARR_SIZE 64
#define FILE_ARR_SIZE (FILE_NAME_MAX_LEN + 1)

//Forward declearations
class SafeFile;

//Different types of PipePackets
typedef enum __PP_Type { 
	READ_REQUEST, WRITE_REQUEST, FINISH_READ, 
	FINISH_WRITE, READ_ACCESS_GRANTED,
	WRITE_ACCESS_GRANTED, ERROR_FILE_DNE
} PP_Type;


//What will be sent through the pipes
class PipePacket {
public: 

	//Constructor
	PipePacket() = delete;
	PipePacket(PP_Type typ, const char * w, const char * n, SafeFile * = NULL);

	//Type of PipePacket
	const PP_Type type;

	//Getters
	const char * getWho() const;
	const char * getName() const;

	//Representation
	SafeFile * file;

private:

	//Representation
	char who[WHO_ARR_SIZE];
	char name[FILE_ARR_SIZE];
};

#endif
