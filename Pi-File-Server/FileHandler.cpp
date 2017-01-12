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
ShmemAllocator * FileHandler::allocIntSet = nullptr;
managed_shared_memory * FileHandler::segment = nullptr;


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
void FileHandler::setup() { RUN_ONCE

	//Prevent memory name using previous memory 
	destroy(); 

	//TODO
	string tmp = fileList+string("\n")+logFile+string("\n");
	ofstream o1( logFile, ios::binary | ios::app ); 
	ofstream o2( fileList, ios::binary ); o2.write( tmp.c_str(), tmp.size() ); 
	ofstream o3( userList, ios::binary ); 
	o1.close(); o2.close(); o3.close();

	//Prevent memory name collision
	destroy(); 

	//Allocate shared memory
	FileHandler::segment = new managed_shared_memory(create_only, SHARED_MEM_NAME, SHARE_MEM_SIZE);

	//Create IntSet allocator
	allocIntSet = new ShmemAllocator(segment->get_segment_manager());

	TMP_MAC(fileList);
	TMP_MAC(logFile);
	named_mutex w(create_only, (wMutexPrefix+userList).c_str());

	//Add the master process as a user
	addMe();

	//TODO

}


//---------------------------------Clean up---------------------------------


//Remove shared memory
void FileHandler::destroy() {

	//Prevent more than two uses
	static int Finished = 0;
	Assert(Finished < 2, "FileHandler::destroy() should only run twice!");
	Finished++;

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
void FileHandler::userQuit(const string& s) { RUN_ONCE

	//Get the user's list of accessed files
	auto users = *(segment->find<IntSet>((userPrefix+s).c_str()).first);

	//Read the file list
	auto files = readAndParse(fileList, true);

	//Revoke access to each file
	for( int i : users ) {
		if (i > 0) finishReadingP((*files)[i-1], s);
		else finishWritingP((*files)[-i-1]);
	}

	//Prevent leaks
	delete files;
}


//----------------------------Blocking functions----------------------------


//Get permission to use a file
void FileHandler::getWriteAccess(const std::string& s) {
	Assert(legalFile(s), "cannot call getWriteAccess on this file!");
	getWriteAccessP(s);
}
void FileHandler::getReadAccess(const std::string& s) {
	Assert(legalFile(s), "cannot call getReadAccess on this file!");
	getReadAccessP(s);
}

//Relinquish access
void FileHandler::finishReading(const std::string& s, std::string who) {

	//Determine who
	if (who == "") who = me();

	//Have who finish reading
	Assert(legalFile(s), "cannot call finishWriting on this file!");
	finishReadingP(s, who);
}
void FileHandler::finishWriting(const std::string& s) {
	Assert(legalFile(s), "cannot call finishReading on this file!");
	finishWritingP(s);
}


//------------------------Blocking wrapper functions------------------------


//Used to log an action
void FileHandler::log(const sstr& s) { log(s.str()); }
void FileHandler::log(const char * s) { log(string(s)); }
void FileHandler::log(const string& s) { 

	//Check use
	Assert(s.size(), "trying to log an empty string");

	//Format string
	sstr ss; ss << me() << s << ((s[s.size()-1] == '\n') ? "":"\n");
	string str = ss.str();

	//Log the string	
	writeP( logFile, str.data(), (int)str.size(), false );
}

//Read data from a file
const data FileHandler::read(const string& s) {

	//Check usage
	Assert(legalFile(s), "cannot read this file!");

	//Get read access
	getReadAccessP(s);

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

//Public wrappers to writeFn
void FileHandler::append(const string& s, const data& d) {
	writeP( s, (char*) &d[0], (int)d.size(), true );
}
void FileHandler::append(const string& s, const string d) {
	writeP( s, (char*) &d[0], (int)d.size(), true );
}
void FileHandler::overWrite(const string& s, const data& d) {
	writeP( s, (char*) &d[0], (int)d.size(), true, false );
}
void FileHandler::overWrite(const string& s, const string d) {
	writeP( s, (char*) &d[0], (int)d.size(), true, false );
}

//Add a user
void FileHandler::addMe() {

	//Aquire ownership of userList
	named_mutex m(open_only, (wMutexPrefix+userList).c_str()); m.lock();

	//Read the file
	ifstream f( userList, ios::binary );
	string str((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
	f.close();

	//Get me with a trailing \n
	string s(std::move(me())); s += '\n';

	//If user already exists, do nothing
	 if (str.find(s) != string::npos) {
		m.unlock(); return;
	}
	
	//Remove the newline
	s.pop_back();

	//Append new user to the list 
	ofstream outFile( userList, ios::binary | ios::app );
	outFile.write( (s+"\n").c_str(), 1+s.size() );
	outFile.close();

	//Create user's file list
	segment->construct<IntSet>((userPrefix+s).c_str()) (less<int>(), *allocIntSet);

	//Relinquish ownership of userList
	m.unlock();
}


//------------------------Blocking 'worker' functions-----------------------


//Get permission to write to the file
void FileHandler::getWriteAccessP(const string& s) {

	//File index
	int i;

	//If the file doesn't exist
	if (!itemExists(s, true, &i)) {

		//Get write access to the file list
		getWriteAccessP(fileList);

		//Re-Check to see if file exists, if not, make it
		if(!itemExists(s, false, &i)) {

			//Add the file to the fileList
			ofstream outFile( fileList, ios::binary | ios::app );
			outFile.write( (s+"\n").data() , 1 + s.size() );
			outFile.close();
		
			//Create and initalize the file's shared memory
			segment->construct<IntSet>((filePrefix+s).c_str()) (less<int>(), *allocIntSet);
			named_mutex editM(create_only, (editMutexPrefix+s).c_str());
			named_mutex wMutex(create_only, (wMutexPrefix+s).c_str());
			wMutex.lock();

			//Check for errors
			Assert(itemExists(s, false, &i), "getWriteAccessP() failed");
		}

		//Relinquish write access
		finishWritingP(fileList);
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
void FileHandler::getReadAccessP(const string& s) {

	//Get the user index
	int i, usr; Assert((bool)itemExists(me(), true, &usr, true), "user doesn't exist!");

	//Add user index to this file's list of users safely
	named_mutex wMutex(open_only, (wMutexPrefix+s).c_str()); wMutex.lock();
	named_mutex editM(open_only, (editMutexPrefix+s).c_str()); editM.lock();
	(segment->find<IntSet>((filePrefix+s).c_str()).first)->insert(usr);
	editM.unlock(); wMutex.unlock();

	//Record the user's access to this file
	Assert(itemExists(s, false, &i), "getReadAccessP called without access");
	(segment->find<IntSet>((userPrefix+me()).c_str()).first)->insert(i+1);
}

//Relinquish reading access to a file
void FileHandler::finishReadingP(const string& s, const string& who) {

	//Get the user index
	int usr; Assert(itemExists(who, true, &usr, true), "user doesn't exist!");

	//Remove user from this file's list of users
	named_mutex editM(open_only, (editMutexPrefix+s).c_str()); editM.lock();
	(segment->find<IntSet>((filePrefix+s).c_str()).first)->erase(usr);
	editM.unlock();

	//Prevent infinite recursion
	int i; if (s == fileList) i = 0;

	//For efficiency
	else if (s == logFile) i = 1;

	//Record the user's loss of access to this file
	else Assert(itemExists(s, true, &i), "finishReadingP called on bad file");
	(segment->find<IntSet>((userPrefix+who).c_str()).first)->erase(i+1);
}

//Relinquish writing access to a file
void FileHandler::finishWritingP(const string& s) {

	//Relinquish writing access to a file
	named_mutex wMutex(open_only, (wMutexPrefix+s).c_str()); wMutex.unlock();

	//Record the user's loss of access to this file
	int i; Assert(itemExists(s, true, &i), "finishWritingP called on bad file");
	(segment->find<IntSet>((userPrefix+me()).c_str()).first)->erase(-(i+1));
}

//Write data to a file
void FileHandler::writeP(const string& s, const char * d, const int n,
							const bool safe, const bool app) {

	//Check usage
	if (safe) Assert(legalFile(s), "cannot write to this file!");

	//Get write access
	FileHandler::getWriteAccessP(s);
	
	//Append data to the file
	auto mode = app ? (ios::binary | ios::app) : ios::binary;
	ofstream outFile( s, mode );
	outFile.write( d , n );
	outFile.close();

	//Relinquish access
	FileHandler::finishWritingP(s);
}


//-----------------------------Helper functions-----------------------------

bool FileHandler::legalFile( const string& s ) {
	return (s != logFile) && (s != fileList) && (s != userList);
}

//-------------------------Blocking helper functions------------------------


//Read and parse file
vector<string> * FileHandler::readAndParse(const string& fileName, const bool getAccess) {

	//Check usage
	if (fileName == FileHandler::userList)
		Assert(getAccess, "User file is a special file. Ownership should always be aquired");

	//Get permission to read the file
	named_mutex * m = nullptr;
	if (fileName == FileHandler::userList) {
		m = new named_mutex(open_only, (FileHandler::wMutexPrefix+fileName).c_str());
		m->lock();
	}
	else if (getAccess) FileHandler::getReadAccessP(fileName);

	//Read the file
	ifstream f( fileName, ios::binary );
	string str((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
	f.close();

	//Finish reading
	if (fileName == FileHandler::userList) m->unlock();
	else if (getAccess) FileHandler::finishReadingP(fileName, me());

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
//If index != nullptr, the index the items is at will be stored in index
//and userList will be used instead of fileList. If getAccess, and we
//are reading from the fileList, get read access before reading the file
bool FileHandler::itemExists( const string& s, const bool getAccess, 
                              int * index, bool usrLst ) {

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

