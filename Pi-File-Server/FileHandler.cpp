#include "FileHandler.hpp"

#include <boost/interprocess/sync/named_mutex.hpp>
#include <fstream>
#include <memory>

//The name of shared memory
#define SHARED_MEM_NAME "PFS-SHARED-MEM"

//For ease of reading
using namespace boost::interprocess;
using namespace std;

//How to name interprocess items
const string FileHandler::filePrefix = "F";
const string FileHandler::userPrefix = "U";
const string FileHandler::wMutexPrefix = "W";
const string FileHandler::editMutexPrefix = "E";

//Define names of needed server files
const string FileHandler::logFile = "LogFile";
const string FileHandler::fileList = "FileList";
const string FileHandler::userList = "UserList";

//Initalize shared memory pointers
ShmemAllocator * FileHandler::allocIntSet = NULL;
managed_shared_memory * FileHandler::segment = NULL;

//Forward declerations
inline vector<string> * readAndParse(const string&, const bool);

/*	A quick note. Shared memory user's file lists are formated as follows
	
	if (reading access) set contains (index + 1)
	if (writing access) set contains -(index + 1)

	The plus 1 above allows the distinguishing between
		reading and writing access to file index 0
*/


//-----------------------------------Setup----------------------------------


//Create the file s' shared memory
#define TMP_MAC(str) {\
	segment->construct<IntSet>((filePrefix+str).c_str()) (less<int>(), *allocIntSet); \
	named_mutex e(create_only, (editMutexPrefix+str).c_str()); \
	named_mutex w(create_only, (wMutexPrefix+str).c_str()); \
}

//Set everything up, called by master process
void FileHandler::setup() {

	//TODO
	string tmp = fileList+string("\n")+logFile+string("\n");
    ofstream o1( logFile, ios::binary | ios::app ); o1.close();
    ofstream o2( fileList, ios::binary ); o2.write( tmp.c_str(), tmp.size() ); o2.close();
    ofstream o3( userList, ios::binary ); o3.close();

	//Prevent memory name collision
	destroy(); 

	//Allocate shared memory
	FileHandler::segment = new managed_shared_memory(create_only, SHARED_MEM_NAME, SHARE_MEM_SIZE);

	//Create IntSet allocator
	allocIntSet = new ShmemAllocator(segment->get_segment_manager());

	TMP_MAC(fileList);
	TMP_MAC(logFile);
	named_mutex w(create_only, (wMutexPrefix+userList).c_str());

	addUser(me());

	//TODO


}


//---------------------------------Clean up---------------------------------


//Remove shared memory
void FileHandler::destroy() {

	//Remove all file's user lists
	shared_memory_object::remove(SHARED_MEM_NAME);

	//Read fileList
	ifstream inFile( fileList, ios::binary );

	//Remove userList ownership mutex
	named_mutex::remove((wMutexPrefix+userList).c_str());

	//Remove all shared memory
	for(string s; getline(inFile, s); ) {
		if (s == "") break;
		named_mutex::remove((wMutexPrefix+s).c_str());
		named_mutex::remove((editMutexPrefix+s).c_str());
	}

	//Close fileList
	inFile.close();
}

//Remove all file access user has, this is blocking
void FileHandler::userQuit(const string& s) {

	//Get the user's list of accessed files
	auto users = *(segment->find<IntSet>((userPrefix+s).c_str()).first);

	//Read the file list
	auto files = readAndParse(fileList, true);

	//Revoke access to each file
	for( int i : users ) {
		if (i > 0) finishReading((*files)[i-1], s);
		else finishWriting((*files)[-i-1]);
	}

	//Prevent leaks
	delete files;
}


//----------------------------Blocking functions----------------------------


//Read and parse file
inline vector<string> * readAndParse(const string& fileName, const bool getAccess) {

	//Check usage
	if (fileName == FileHandler::userList)
		Assert(getAccess, "User file is a special file. Ownership should always be aquired");

	//Get permission to read the file
	named_mutex * m = NULL;
	if (fileName == FileHandler::userList) {
		m = new named_mutex(open_only, (FileHandler::wMutexPrefix+fileName).c_str());
		m->lock();
	}
	else if (getAccess) FileHandler::getReadAccess(fileName);

	//Read the file
	ifstream f( fileName, ios::binary );
	string str((istreambuf_iterator<char>(f)), istreambuf_iterator<char>()); f.close();

	//Finish reading
	if (fileName == FileHandler::userList) m->unlock();
	else if (getAccess) FileHandler::finishReading(fileName);

	//Parse string
	auto ret = new vector<string>();
	for(int i=0,k=0; k < (int) str.size(); k++)
		if (str[k] == '\n') {
			ret->push_back(str.substr(i,k-i));
			i = k+1;
		}

	//Return the result
	return ret;
}

//Check if an item exists in file f (either userList or fileList)
//If index != NULL, the index the items is at will be stored in index
//and userList will be used instead of fileList. If getAccess, and we
//are reading from the fileList, get read access before reading the file
inline bool itemExists(const string& s, const bool getAccess = true,
						int * index = NULL, bool usrLst = false) {

	//Determine which file to use
	string fileName = usrLst ? FileHandler::userList : FileHandler::fileList;

	//Read and parse the file
	unique_ptr<vector<string> > files(readAndParse(fileName, getAccess));

	//Serach file list for the file
	//If found, set *index = i if needed, then return true
	for(int i = 0; i < (int) files->size(); i++)
		if ((*files)[i] == s) { 
			if (index) *index = i; 
			return true;
		}

	//Prevent leaks and fail
	return false;
}

//Get permission to write to the file
void FileHandler::getWriteAccess(const string& s) {

	//File index
	int i;

	//If the file doesn't exist
	if (!itemExists(s, true, &i)) {

		//Get write access to the file list
		getWriteAccess(fileList);

		//Re-Check to see if file exists, if not, make it
		if(!itemExists(s, true, &i)) {
		
			//Create and initalize the file's shared memory
			segment->construct<IntSet>((filePrefix+s).c_str()) (less<int>(), *allocIntSet);
			named_mutex editM(open_or_create, (editMutexPrefix+s).c_str());
			named_mutex wMutex(open_or_create, (wMutexPrefix+s).c_str());
			wMutex.lock();

			//Add the file to the fileList
			ofstream outFile( s, ios::binary | ios::app );
			outFile.write( (s+"\n").data() , 1 + s.size() );
			outFile.close();

			//Check for errors
			Assert(itemExists(s, true, &i), "getWriteAccess() failed");
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
	auto users = segment->find<IntSet>((filePrefix+s).c_str()).first;
	while (users->size()) usleep((int)10000);

	//Record that user's access to this file
	(segment->find<IntSet>((userPrefix+me()).c_str()).first)->insert(-(i+1));
}

//Get permission to read a file
void FileHandler::getReadAccess(const string& s) {

	//Get the user index
	int i, usr; Assert((bool)itemExists(me(), true, &usr, true), "User doesn't exist!");

	//Add user index to this file's list of users safely
	named_mutex wMutex(open_only, (wMutexPrefix+s).c_str()); wMutex.lock();
	named_mutex editM(open_only, (editMutexPrefix+s).c_str()); editM.lock();
	(segment->find<IntSet>((filePrefix+s).c_str()).first)->insert(usr);
	editM.unlock(); wMutex.unlock();

	//Record the user's access to this file
	Assert(itemExists(s, false, &i), "getReadAccess called without access");
	(segment->find<IntSet>((userPrefix+me()).c_str()).first)->insert(i+1);
}

//Relinquish reading access to a file
void FileHandler::finishReading(const string& s, string who) {

	//Determine who
	if (who == "") who = me();

	//Get the user index
	int usr; Assert(itemExists(who, true, &usr, true), "User doesn't exist!");

	//Remove user from this file's list of users
	named_mutex editM(open_only, (editMutexPrefix+s).c_str()); editM.lock();
	(segment->find<IntSet>((filePrefix+s).c_str()).first)->erase(usr);
	editM.unlock();

	//Prevent infinite recursion
	int i; if (s == fileList) i = 0;

	//For efficiency
	else if (s == logFile) i = 1;

	//Record the user's loss of access to this file
	else Assert(itemExists(s, true, &i), "finishReading called on bad file");
	(segment->find<IntSet>((userPrefix+who).c_str()).first)->erase(i+1);
}

//Relinquish writing access to a file
void FileHandler::finishWriting(const string& s) {

	//Relinquish writing access to a file
	named_mutex wMutex(open_only, (wMutexPrefix+s).c_str()); wMutex.unlock();

	//Record the user's loss of access to this file
	int i; Assert(itemExists(s, true, &i), "finishWriting called on bad file");
	(segment->find<IntSet>((userPrefix+me()).c_str()).first)->erase(-(i+1));
}


//------------------------Blocking wrapper functions------------------------


//Read data from a file
const data FileHandler::read(const string& s) {

	//Get read access
	getReadAccess(s);

	//Read data in
	ifstream inFile( s, ios::binary );
	data ret( (istreambuf_iterator<char>(inFile)), 
              (istreambuf_iterator<char>()) );
	inFile.close();

	//Relinquish access
	finishReading(s);
	
	//Return data
	return ret;	
}


//Write data to a file
inline void writeFn(const string& s, const char * d, int n) {

	//Get write access
	FileHandler::getWriteAccess(s);
    
    //Append data to the file
    ofstream outFile( s, ios::binary | ios::app );
    outFile.write( d , n*sizeof(char) );
	outFile.close();

	//Relinquish access
	FileHandler::finishWriting(s);
}

//Public wrappers to writeFn
void FileHandler::write(const string& s, const data& d) {
	writeFn( s, (char*) &d[0], (int)d.size() );
}
void FileHandler::write(const string& s, const string d) {
	writeFn( s, (char*) &d[0], (int)d.size() );
}

//Add a user
void FileHandler::addUser(const string& s) {

	//Aquire ownership of userList
	named_mutex m(open_only, (wMutexPrefix+userList).c_str()); m.lock();

    //Append new user to the list 
    ofstream outFile( userList, ios::binary | ios::app );
    outFile.write( (s+"\n").c_str(), 1+s.size() );
	outFile.close();

	//Create user's file list
	segment->construct<IntSet>((userPrefix+s).c_str()) (less<int>(), *allocIntSet);

	//Relinquish ownership of userList
	m.unlock();
}

