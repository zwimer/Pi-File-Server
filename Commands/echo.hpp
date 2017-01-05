#include "../Pi-File-Server/AbstractCommand.hpp"

class echo : public AbstractCommand<echo> {
public:

	//Constructor
	echo();

	//Destructor
	~echo();

	//Run the command
	std::string execute(std::string& args, std::string& path);

};
