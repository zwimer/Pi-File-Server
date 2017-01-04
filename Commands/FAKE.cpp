//This file is currently fake, included only for testing

#include "FAKE.hpp"

FAKE::FAKE() : AbstractCommand() {}
FAKE::~FAKE() {}

//Run the command
void FAKE::execute(const std::string& path) {};

//Create another AbstractCommand of the same type
AbstractCommand * FAKE::createNew(std::string args) const {return NULL;};

