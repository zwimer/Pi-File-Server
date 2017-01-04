//This file is currently fake, included only for testing

#include "../Pi-File-Server/AbstractCommand.hpp"

class FAKE : public AbstractCommand {
public:

	FAKE();
	~FAKE();

	//Run the command
	void execute(const std::string& path);

	//Create another AbstractCommand of the same type
	AbstractCommand * createNew(std::string args) const;

};
