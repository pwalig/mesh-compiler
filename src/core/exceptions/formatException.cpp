#include "formatException.h"
#include <iostream>
#include <string>

std::ostream& operator<<(std::ostream& os, const mc::formatException& fe)
{
	std::cerr << "format interpretation error: in line " + std::to_string(fe.context.linenum)
		+ " " + fe.what();
    return os;
}
