//This file is currently fake, included only for testing

#include "FAKE.hpp"

FAKE::FAKE() : AbstractCommand() {}
FAKE::~FAKE() {}

//Run the command
std::string FAKE::execute(std::string& args, std::string& path) {return ""};

//Create another AbstractCommand of the same type
AbstractCommand * FAKE::createNew() const {return NULL;};

