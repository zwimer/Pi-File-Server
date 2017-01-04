#include "CHException.hpp"

//Constructor
CHException::CHException(enum CHException::EType e) : which(e) {}

//Get type of exception
enum CHException::EType CHException::type() const { return which; }
