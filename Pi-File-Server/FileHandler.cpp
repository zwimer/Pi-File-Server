#include "FileHandler.hpp"

#include <boost/interprocess/sync/named_mutex.hpp>
#include <fstream>

//The name of shared memory
#define SHARED_MEM_NAME "PFS-SHARED-MEM"

//For ease of reading
using namespace boost::interprocess;

//How to name interprocess items
const std::string FileHandler::filePrefix = "F";
const std::string FileHandler::wMutexPrefix = "M";
const std::string FileHandler::editMutexPrefix = "E";

//Define names of needed server files
const std::string FileHandler::logFile = "LogFile";
const std::string FileHandler::fileList = "FileList";
const std::string FileHandler::userList = "UserList";


//-----------------------------------Setup----------------------------------


//Set everything up
void FileHandler::setup() {

	//Prevent memory name collision
	destroy(); 

	//Allocate shared memory
	segment = new managed_shared_memory(create_only, SHARED_MEM_NAME, SHARE_MEM_SIZE);

	//Create IntSet allocator
	allocIntSet = new ShmemAllocator(segment->get_segment_manager());


	//TODO


}

//Remove shared memory
void FileHandler::destroy() {
	shared_memory_object::remove(SHARED_MEM_NAME);
}


//----------------------------Blocking functions----------------------------


//Check if an item exists in file f (either userList or fileList)
//If index != NULL, the index the items is at will be stored in index
//and userList will be used instead of fileList. If getAccess, and we
//are reading from the fileList, get read access before reading the file
inline bool itemExists(const std::string& s, const bool getAccess = true, int * index = NULL) {

	//Determine which file to use
	std::string fileName = index ? FileHandler::userList : FileHandler::fileList;

	//Read the file list
	if (getAccess && !index) FileHandler::getReadAccess(fileName);
	std::ifstream f( s, std::ios::binary );
	std::string str((std::istreambuf_iterator<char>(f)),
					 std::istreambuf_iterator<char>()); f.close();
	if (getAccess && !index) FileHandler::finishReading(fileName);

	//Parse string for ease
	std::vector<std::string> files;
	for(int i=0,k=0; k < (int) str.size(); k++)
		if (str[k] == '\n') {
			files.push_back(str.substr(i,k-i));
			i = k+1;
		}

	//Serach file list for the file
	//If found, set *index = i if needed,
	//then return true. Otherwise, return false.
	for(int i = 0; i < (int) files.size(); i++)
		if (files[i] == s) { *index = i; return true; }
	return false;
}

//Get permission to write to the file
void FileHandler::getWriteAccess(const std::string& s) {

	//If the file doesn't exist
	if (!itemExists(s)) {

		//Get write access to the file list
		getWriteAccess(fileList);

		//Re-Check to see if file exists, if not, make it
		if(!itemExists(s, true)) {
		
			//Create and initalize the file's shared memory
			segment->construct<IntSet>((filePrefix+s).c_str()) (std::less<int>(), allocIntSet);
			named_mutex editM(open_or_create, (editMutexPrefix+s).c_str());
			named_mutex wMutex(open_or_create, (wMutexPrefix+s).c_str());
			wMutex.lock();

			//Add the file to the fileList
			std::ofstream outFile( s, std::ios::binary | std::ios::app );
			outFile.write( s.data() , s.size() ); outFile.write( "\n", 1 );
			outFile.close();
		}

		//Relinquish write access
		finishWriting(fileList);
	}

	//If the file exists, lock it
	else {
		named_mutex wMutex(open_only, (wMutexPrefix+s).c_str());
		wMutex.lock();
	}

	//Wait for there to be no readers
	auto userList = *(segment->find<IntSet>((filePrefix+s).c_str()).first);
	while (userList.size()) { sleep(.001); }
}

//Get permission to read a file
void FileHandler::getReadAccess(const std::string& s) {

	//Get the user index
	named_mutex uMutex(open_only, (wMutexPrefix+s).c_str()); uMutex.lock();
	int usr; Assert(itemExists(s, true, &usr), "User doesn't exist!");
	uMutex.unlock();

	//Add user index to this file's list of users safely
	named_mutex wMutex(open_only, (wMutexPrefix+s).c_str()); wMutex.lock();
	named_mutex editM(open_only, (editMutexPrefix+s).c_str()); editM.lock();
	(segment->find<IntSet>((filePrefix+s).c_str()).first)->insert(usr);
	editM.unlock(); wMutex.unlock();
}

//Relinquish reading access to a file
void FileHandler::finishReading(const std::string& s) {

	//Get the user index
	named_mutex uMutex(open_only, (wMutexPrefix+s).c_str()); uMutex.lock();
	int usr; Assert(itemExists(s, true, &usr), "User doesn't exist!");
	uMutex.unlock();

	//Remove user from this file's list of users
	named_mutex editM(open_only, (editMutexPrefix+s).c_str()); editM.lock();
	(segment->find<IntSet>((filePrefix+s).c_str()).first)->erase(usr);
	editM.unlock();
}

//Relinquish writing access to a file
void FileHandler::finishWriting(const std::string& s) {
	named_mutex wMutex(open_only, (wMutexPrefix+s).c_str()); wMutex.unlock();
}


//------------------------Blocking wrapper functions------------------------


//Read data from a file
const data FileHandler::read(const std::string& s) {

	//Get read access
	getReadAccess(s);

	//Read data in
	std::ifstream inFile( s, std::ios::binary );
	data ret( (std::istreambuf_iterator<char>(inFile)), 
              (std::istreambuf_iterator<char>()) );
	inFile.close();

	//Relinquish access
	finishReading(s);
	
	//Return data
	return ret;	
}


//Write data to a file
inline void writeFn(const std::string& s, const char * d, int n) {

	//Get write access
	FileHandler::getWriteAccess(s);
    
    //Append data to the file
    std::ofstream outFile( s, std::ios::binary | std::ios::app );
    outFile.write( d , n*sizeof(char) );
	outFile.close();

	//Relinquish access
	FileHandler::finishWriting(s);
}

//Public wrappers to writeFn
void FileHandler::write(const std::string& s, const data& d) {
	writeFn( s, (char*) &d[0], (int)d.size() );
}
void FileHandler::write(const std::string& s, const std::string d) {
	writeFn( s, (char*) &d[0], (int)d.size() );
}

