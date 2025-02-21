#pragma once
#include <unordered_map>
#include <string>

#include "stype.h"
#include "ctype.h"

namespace mc {
	namespace vtype {
		enum code : char {
            null,

            indice,
            vertex,
            normal,
            tangent,
            bitangent,
            uv,
            vertex_color,
            bone_id,
            bone_weight,

            mesh_bone_offset_matrix,
            mesh_bone_parent,
            mesh_bone_child,

            offset_matrix,
            bone_parent,
            bone_child,

            position_key,
            rotation_key,
            scale_key,
            position_key_timestamp,
            rotation_key_timestamp,
            scale_key_timestamp,

            duration,
            ticks_per_second
		};

        extern const std::unordered_map<std::string, code> codes;
        extern const std::unordered_map<code, std::string> names;
        extern const std::unordered_map<code, std::vector<unsigned short>> maxSuffixes;
        extern const std::unordered_map<code, stype::code> default_stypes;
        extern const std::unordered_map<code, ctype::code> ctypes;
	}
}