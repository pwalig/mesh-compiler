#pragma once
#include <stdexcept>

namespace mc {
	class compileException : public std::runtime_error {
	public:
		compileException(const char* _Message) : runtime_error(_Message) {}
		compileException(const std::string _Message) : runtime_error(_Message) {}
	};
}
