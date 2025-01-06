#include "field.h"
#include "fields/cfield.h"
#include "fields/pfield.h"
#include "fields/vfield.h"
#include "exceptions/jsonException.h"

mc::field::ptr mc::field::getPtr(const rapidjson::Value& json, location loc)
{
	if (!json.IsObject()) throw jsonException("field was not json object");
	if (!json.HasMember("value")) throw jsonException("field json object did not have member \"value\"");

	if (json["value"].IsNumber()) {
		if (!json.HasMember("type")) throw jsonException("field json object did not have member \"type\"");
		if (!json["type"].IsString()) throw jsonException("field json object's member \"type\" was not a string");
		return cfield::getPtr(json);
	}
	else {
		if (!json["value"].IsString()) throw jsonException("value was neither a number nor a string");
		std::string value = json["value"].GetString();

		if (loc == location::main_preamble) {
			if (ptype::codes.find(value) != ptype::codes.end()) {
				return ptr(new pfield(stype::codes.at(json["type"].GetString()), ptype::codes.at(value)));
			}
			throw jsonException("unknown value: " + value);
		}
		else if (loc == location::buffer_preamble) {

			if (ptype::codes.find(value) != ptype::codes.end()) {
				ptype::code pc = ptype::codes.at(value);
				if (ptype::buffer_allowed.find(pc) == ptype::buffer_allowed.end())
					throw jsonException("ptype: " + value + " unallowed in buffer preamble");
				return ptr(new bpfield(stype::codes.at(json["type"].GetString()), ptype::codes.at(value)));
			}
			throw jsonException("unknown value: " + value);
		}
		else {
			assert(loc == location::buffer_field);
			return vfield::getPtr(json);
		}
	}
}
