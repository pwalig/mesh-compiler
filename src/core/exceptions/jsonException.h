#pragma once
#include <stdexcept>

namespace mc {
	class jsonException : public std::logic_error {
	public:
		jsonException(const char* _Message) : logic_error(_Message) {}
		jsonException(const std::string _Message) : logic_error(_Message) {}
	};
}
