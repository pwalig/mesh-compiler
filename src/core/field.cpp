#include "field.h"
#include "fields/cfield.h"
#include "fields/pfield.h"
#include "fields/vfield.h"
#include "fields/ufield.h"
#include "exceptions/jsonException.h"
#include "suffixes.h"
#include "exceptions/formatException.h"
#include "compilation-info.h"

size_t mc::field::getCount() const
{
	return 1;
}

mc::field::ptr mc::field::getPtr(const rapidjson::Value& json, location loc)
{
	if (json.IsString()) return mc::field::getPtr(json.GetString(), loc);
	if (!json.IsObject()) throw jsonException("field was not json object");
	if (!json.HasMember("value")) throw jsonException("field json object did not have member \"value\"");


	if (!json.HasMember("type")) {
		if (!json["value"].IsString()) throw jsonException("field json object's member \"value\" was not a string when no member \"type\" was provided");
		return field::ptr(new ufield(json["value"].GetString()));
	}
	if (!json["type"].IsString()) throw jsonException("field json object's member \"type\" was not a string");
	std::string typestr = json["type"].GetString();
	if (stype::codes.find(typestr) == stype::codes.end()) throw jsonException("unknown type: " + typestr);
	stype::code typec = stype::codes.at(typestr);

	if (json["value"].IsNumber()) {
		return cfield::getPtr(json);
	}
	else {
		if (!json["value"].IsString()) throw jsonException("value was neither a number nor a string");
		std::string valuestr = json["value"].GetString();

		if (loc == location::main_preamble) {
			if (ptype::codes.find(valuestr) != ptype::codes.end()) {
				return ptr(new pfield(typec, ptype::codes.at(valuestr)));
			}
		}
		else if (loc == location::buffer_preamble) {

			if (ptype::codes.find(valuestr) != ptype::codes.end()) {
				ptype::code pc = ptype::codes.at(valuestr);
				if (ptype::buffer_allowed.find(pc) == ptype::buffer_allowed.end())
					throw jsonException("ptype: " + valuestr + " unallowed in buffer preamble");
				return ptr(new bpfield(typec, ptype::codes.at(valuestr)));
			}
		}
		else {
			assert(loc == location::buffer_field);

			auto suffixes = mc::extractSuffixes(valuestr);
			std::string vtype_str = vtypeNoSuffix(valuestr);
			if (vtype::codes.find(vtype_str) != vtype::codes.end()) {
				return field::ptr(new vfield(typec, vtype::codes.at(vtype_str), suffixes));
			}
		}

		throw jsonException("unknown value: " + valuestr);
	}
}

mc::field::ptr mc::field::getPtr(const std::string& word, location loc, const compilationContext& context)
{
	size_t pos = word.find_first_of(':'); // type separator
	stype::code st = stype::null; // size type of the field
	std::string valuestr = word;
	if (pos != std::string::npos) {
		st = stype::codes.at(word.substr(0, pos));
		valuestr = word.substr(pos + 1, word.size() - pos);

		try {
			anyType::value v = anyType::get(st, valuestr);
			return ptr(new cfield(st, v));
		}
		catch (std::invalid_argument&) {}
	}

	if (loc == location::main_preamble) {
		if (ptype::codes.find(valuestr) != ptype::codes.end()) {
			ptype::code pt = ptype::codes.at(valuestr);
			if (st == stype::null) st = ptype::default_stypes.at(pt);
			return ptr(new pfield(st, pt));
		}
	}
	else if (loc == location::buffer_preamble) {

		if (ptype::codes.find(valuestr) != ptype::codes.end()) {
			ptype::code pt = ptype::codes.at(valuestr);
			if (ptype::buffer_allowed.find(pt) == ptype::buffer_allowed.end())
				throw formatException("ptype: " + valuestr + " unallowed in buffer preamble", context);
			if (st == stype::null) st = ptype::default_stypes.at(pt);
			return ptr(new bpfield(st, pt));
		}
	}
	else {
		assert(loc == location::buffer_field);

		auto suffixes = mc::extractSuffixes(valuestr);
		std::string vtype_str = vtypeNoSuffix(valuestr);
		if (vtype::codes.find(vtype_str) != vtype::codes.end()) {
			vtype::code vt = vtype::codes.at(vtype_str);
			if (st == stype::null) st = vtype::default_stypes.at(vt);
			return field::ptr(new vfield(st, vt, suffixes));
		}
	}

	if (st == stype::null &&
		mc::compilationInfo::units.find(valuestr) != mc::compilationInfo::units.end())
		return ptr(new ufield(valuestr));
	else throw formatException("unknown token: " + valuestr, context);
}
