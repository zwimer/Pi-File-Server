#ifndef CMD_HPP
#define CMD_HPP

#include <string>

//A wrapper to the generic AbstractCommand class.
//This class exists so that arrays of AbstractCommand<T>*s
//can be created, where T may differ between elements in array
class Cmd {
public: 

	//Constructor
	Cmd(); virtual ~Cmd();

	//Wrapper to AbstractCommand's createNew
	virtual Cmd * createNew() const = 0;

	//Run the command, return the message to send back
	virtual std::string execute(std::string& args, std::string& path) = 0;	

};

#endif
