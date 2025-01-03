#pragma once
#include <stdexcept>

namespace mc {
	class jsonException : public std::runtime_error {
	public:
		jsonException(const char* _Message) : runtime_error(_Message) {}
		jsonException(const std::string _Message) : runtime_error(_Message) {}
	};
}
