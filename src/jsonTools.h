#pragma once
#include <string>
#include <vector>
#include <rapidjson/document.h>

namespace jsonTools {
	std::string getAnyMember(const rapidjson::Value& json, std::vector<std::string> available);
}
