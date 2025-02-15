#pragma once
#include <stdexcept>
#include <ostream>
#include "../compilationContext.h"

namespace mc {
	class formatException : public std::runtime_error {
	public:
		const compilationContext context;

		formatException(const char* _Message, compilationContext context_) :
			runtime_error(_Message), context(context_) {}
		formatException(const std::string _Message, compilationContext context_) :
			runtime_error(_Message), context(context_) {}
	};
}

std::ostream& operator<<(std::ostream& os, const mc::formatException& fe);
