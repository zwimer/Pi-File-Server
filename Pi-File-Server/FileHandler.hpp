#ifndef FILE_HANDLER_HPP
#define FILE_HANDLER_HPP


//Forward declearations
class SafeFile;

//A static class that handles file IO
class FileHandler {
public:
	
	//-------------------------Setup------------------------

	//This function scans for files that exist, notes 
	//which exist, and creates any necessary files that don't.
	static void setup();
    
	//------------------Blocking functions------------------

	//Get permission to use a file
	static void getWriteAccess();
	static void getReadAccess();

	//Relinquish access
	static void finishReading();
	static void finishWriting();

	//--------------Blocking wrapper functions--------------

	//Read data from a file
	static const data read(const std::string& s);

	//Write data to a file
	static void write(const std::string& s, const data& d);
	static void write(const std::string& s, const std::string d);

	//-------------------Const variables-------------------

	//Required server files
	static const std::string logFile;
	static const std::string infoFile;
	static const std::string userFile;
};

#endif
