#include "formatException.h"
#include <iostream>
#include <string>

void mc::formatException::print()
{
	std::cerr << "format interpretation error: in line " + std::to_string(this->context.linenum)
		+ " " + this->what() + "\n";
}
