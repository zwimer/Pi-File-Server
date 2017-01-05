#include "../Pi-File-Server/AbstractCommand.hpp"

//Print the arguments given
struct echo : AbstractCommand<echo> {
	std::string execute(std::string& args, std::string& path);
};
