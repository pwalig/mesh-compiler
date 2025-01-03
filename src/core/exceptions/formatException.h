#pragma once
#include <stdexcept>

namespace mc {
	class formatException : public std::runtime_error {
	public:
		formatException(const char* _Message) : runtime_error(_Message) {}
		formatException(const std::string _Message) : runtime_error(_Message) {}
	};
}
