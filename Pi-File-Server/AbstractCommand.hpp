#ifndef ABSTRACT_COMMAND_HPP
#define ABSTRACT_COMMAND_HPP

#include "main.hpp"

/* Important for sub-classes:

All sub-classes MUST contain a factory 
function with the following prototype: 

	AbstractCommand * createNew(std::string args);
	
This function must create another instance of the class.
However, it may NOT execute the command! The command
will execute when the following member function is called

	void execute(const std::string& path);

Important note: The command handler has no knowledge of what
arguments your command does or does not take. It will feed in
the string args, and it will blindly feed in the entire argument
given, even if it does contain garbage info. For example, if the
command was cd, args could be "~/Desktop some garbage data 123..."
Your command is responsible for parsing args and determing what
is and is not important.

 */

//An abstract command
class AbstractCommand {
public:

	//Constructor
	AbstractCommand();
	
	//Destructor
	virtual ~AbstractCommand();

	//Create another AbstractCommand of the same type
	virtual AbstractCommand * createNew(std::string args) = 0;

	//Run the command
	virtual void execute(const std::string& path) = 0;	
};

#endif
