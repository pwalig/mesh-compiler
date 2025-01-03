#pragma once
#include <stdexcept>

namespace mc {
	class formatException : public std::logic_error {
	public:
		formatException(const char* _Message) : logic_error(_Message) {}
		formatException(const std::string _Message) : logic_error(_Message) {}
	};
}
