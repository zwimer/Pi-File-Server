#ifndef ABSTRACT_COMMAND_HPP
#define ABSTRACT_COMMAND_HPP

#include "Cmd.hpp"

/* Important for sub-classes: Sub-class AbstractCommand<T>

	Also: The command handler has no knowledge of what arguments
	your command does or does not take. It will blindly feed into
	the string args, all information it was given, even if it does
	contain garbage info. For example, if the command was cd, 
	args could be "~/Desktop some garbage data 123..." Your 
	command is responsible for parsing args and determing
	what is and is not important.

 */

//A wrapper class to Abstract Command


//An abstract command
template<class T> class AbstractCommand : public Cmd {
public:

	//Constructor
	AbstractCommand();
	
	//Destructor
	virtual ~AbstractCommand();

	//Create another Cmd of type T
	Cmd * createNew() const;

	//Run the command, return the message to send back
	virtual std::string execute(std::string& args, std::string& path) = 0;	

};


//Constructor
template<class T> AbstractCommand<T>::AbstractCommand() : Cmd() {}

//Destructor
template<class T> AbstractCommand<T>::~AbstractCommand() {}

//Create another AbstractCommand of the same type
template<class T> Cmd * AbstractCommand<T>::createNew() const { 
	return new T();
}


#endif
