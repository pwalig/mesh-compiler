#include "jsonTools.h"

std::string jsonTools::getAnyMember(const rapidjson::Value& json, std::vector<std::string> available)
{
    for (const std::string& s : available) {
        if (json.HasMember(s.c_str())) {
            return s;
        }
    }
    return "";
}
