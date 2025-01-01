#include "field.h"
#include "fields/cfieldT.h"
#include "fields/pfieldT.h"
#include "fields/vfield.h"

mc::field::ptr mc::field::getPtr(const rapidjson::Value& json, location loc)
{
	assert(json.IsObject());
	assert(json.HasMember("value"));

	if (json["value"].IsNumber()) {
		return getCFieldTPtr(json);
	}
	else {
		assert(json["value"].IsString());
		std::string value = json["value"].GetString();

		if (loc == location::main_preamble) {
			if (ptype::codes.find(value) != ptype::codes.end()) {
				return ptr(new pfieldT<unsigned int>(ptype::codes.at(value)));
			}
		}
		else if (loc == location::buffer_preamble) {

			if (ptype::codes.find(value) != ptype::codes.end()) {
				ptype::code pc = ptype::codes.at(value);
				if (ptype::buffer_allowed.find(pc) == ptype::buffer_allowed.end())
					throw std::runtime_error("ptype unallowed in buffer preamble");
				return ptr(new bpfieldT<unsigned int>(ptype::codes.at(value)));
			}
		}
		else {
			assert(loc == location::buffer_field);
			assert(vtype::codes.find(value) != vtype::codes.end());
			return vfield::getPtr(json);
		}

	}

	return ptr();
}
