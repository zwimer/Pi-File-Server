#include "../Pi-File-Server/AbstractCommand.hpp"

class echo : public AbstractCommand {
public:

	//Constructor
	echo();

	//Destructor
	~echo();

	//Run the command
	std::string execute(std::string& args, std::string& path);

	//Create another AbstractCommand of the same type
	AbstractCommand * createNew() const;

};
