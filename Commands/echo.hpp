#ifndef ECHO_HPP
#define ECHO_HPP

#include "../Pi-File-Server/AbstractCommand.hpp"

//Print the arguments given
struct echo : AbstractCommand<echo> {
	std::string execute(std::string& args, std::string&);
};

#endif
