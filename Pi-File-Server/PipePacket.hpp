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
	FINISH_WRITE, INIT, READ_ACCESS_GRANTED,
	WRITE_ACCESS_GRANTED, ERROR_FILE_DNE
} PP_Type;



//What will be sent through the pipes
class PipePacket {
public: 

	//Constructor
    PipePacket() = delete;
    PipePacket(const PipePacket * p);
    PipePacket(PP_Type typ, const char * w, const char * n, SafeFile * = NULL);

	//Type of PipePacket
	const PP_Type type;

	//Getters
	const char * getWho() const;
	const char * getName() const;

    //Read and Write wrappers
    void write(int fd) const;
    void read(int fd);
    
    //Calls write, then read, then verifies the response
    void sendRecvVerify(int p[], const PP_Type responce, const char * Msg);
    
    //Representation
    const int ProcessNum;
	SafeFile * file;

private:

	//Representation
	char who[WHO_ARR_SIZE+1];
	char name[FILE_ARR_SIZE+1];
};

//Stream operator
std::ostream& operator << (std::ostream& s, const PipePacket& p);

#endif
