#include "SafeFile.hpp"

//Constructor
SafeFile::SafeFile(const char * n, const std::string& who) : name(n) {
	writing = PTHREAD_MUTEX_INITIALIZER;
	whoM = PTHREAD_MUTEX_INITIALIZER;
	getWriteAccess(who);
}

//Destructor
~SafeFile::SafeFile() {}


//----------------------Synchronized----------------------


//Get permission to read, this functions blocks
void SafeFile::getReadAccess(const std::string& who) {

	//Block if another thread is writing
	pthread_mutex_lock(&writing);

	//And who to the readers list
	pthread_mutex_lock(&whoM);
	users.insert(who);
	pthread_mutex_unlock(&whoM);

	//Unblock writing
	pthread_mutex_unlock(&writing);
}

//Get permission to read, this functions blocks
void SafeFile::getWriteAccess(const std::string& who) {

	//Get the writing lock
	pthread_mutex_lock(&writing);
	
	//Wait until there are no writers
	while(users.size()) sleep(.001);

	//Note down who is the writer
	users.insert(who);
}

//Relinquish reading rights to this file
void SafeFile::finishReading(const std::string& who) {

	//Check for errors
	Assert(users.size(), "finishReading called, users was empty.");
	Assert(users.find(who) != users.end(), "finishReading called by wrong thread.");

	//Remove who from users
	pthread_mutex_lock(&whoM);
	users.erase(who);
	pthread_mutex_unlock(&whoM);
}

//Relinquish reading rights to this file
void SafeFile::finishWriting(const std::string& who) {

	//Check for errors
	Assert(!pthread_mutex_trylock(&writing),
		"finishWriting was called when nothing was writing.");
	Assert(users.size()==1, "finishWriting called, users was the wrong size.");
	Assert(*users.begin() == who, "finishWriting called by wrong thread.");

	//Clear users and unlock writing
	pthread_mutex_lock(&whoM);
	users.clear();
	pthread_mutex_unlock(&whoM);
	pthread_mutex_unlock(&writing);
}

