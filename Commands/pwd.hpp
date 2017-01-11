#ifndef PWD_HPP
#define PWD_HPP

#include "../Pi-File-Server/AbstractCommand.hpp"

//Print the current working directory
struct pwd : AbstractCommand<pwd> {
	std::string execute(std::string&, std::string& path);
};

#endif
