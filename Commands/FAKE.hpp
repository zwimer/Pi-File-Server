//This file is currently fake, included only for testing

#include "../Pi-File-Server/AbstractCommand.hpp"

class FAKE : public AbstractCommand {
public:

	FAKE();
	~FAKE();

	//Run the command
	std::string execute(std::string& args, std::string& path);

	//Create another AbstractCommand of the same type
	AbstractCommand * createNew() const;

};
