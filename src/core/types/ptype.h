#pragma once
#include <unordered_map>
#include <string>
#include "stype.h"
#include <unordered_set>

namespace mc {
	namespace ptype {
		enum code : char {
            null,
            unit_size,
            buffer_size,
            buffers_per_unit,
            entry_size,
            entries_per_unit,
            entries_per_buffer,
            field_size,
            fields_per_unit,
            fields_per_buffer,
            fields_per_entry
		};

        extern const std::unordered_map<code, std::string> names;
        extern const std::unordered_map<std::string, code> codes;
        extern const std::unordered_set<code> buffer_allowed;
        extern const std::unordered_map<code, stype::code> default_stypes;
	}
}