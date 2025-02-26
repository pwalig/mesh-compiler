#pragma once
#include <stdexcept>
#include <ostream>
#include "../formatInterpreterContext.h"

namespace mc {
	class formatException : public std::runtime_error {
	public:
		const formatInterpreterContext context;

		formatException(const char* _Message, formatInterpreterContext context_) :
			runtime_error(_Message), context(context_) {}
		formatException(const std::string _Message, formatInterpreterContext context_) :
			runtime_error(_Message), context(context_) {}
	};
}

std::ostream& operator<<(std::ostream& os, const mc::formatException& fe);
