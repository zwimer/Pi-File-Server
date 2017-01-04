#ifndef CHEXCEPTION_HPP
#define CHEXCEPTION_HPP

#include <exception>

//A custom exception thrown by CommandHandler
class CHException : public std::exception {
public:

	//Types of exceptions
	enum EType { NO_COMMAND, INVALID_COMMAND };

	//Constructor
	CHException()=delete;
	CHException(enum EType e);

	//Get type of exception
	enum EType type() const;

private:

	//Representation	
	const enum EType which;
};

#endif
