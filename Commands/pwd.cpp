#include "pwd.hpp"

//Print the current working directory
std::string pwd::execute(std::string&, std::string& pth) {
	return pth + '\n';
};

