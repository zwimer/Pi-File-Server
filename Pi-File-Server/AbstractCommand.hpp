#include "main.hpp"

/* Important for sub-classes:

All sub-classes MUST contain a factory 
function with the following prototype: 

	AbstractCommand * createNew(std::string& args);

This function must create an instance of the class.
However, it may NOT execute the command! The command
will execute when the following member function is called

	void execute(std::string path);

Overall, how the command will be called will be as follows:

	AbstractCommand * cmd = <MyCommandName>::createNew;
	cmd->execute(<Current Directory>);
	delete cmd;

 */

//An abstract command
class AbstractCommand {
public:

	//Constructor
	AbstractCommand();


	//Run the command
	virtual void execute(std::string path) = 0;	



	





};
