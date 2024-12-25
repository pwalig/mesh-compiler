#pragma once
#include <unordered_map>
#include <string>

namespace mc {
	namespace ctype { // counting type
        enum code {
            null,

            per_indice,
            per_vertex,
            per_mesh_bone,
            per_mesh,

            per_bone,
            per_skeleton,

            per_position_keyframe,
            per_rotation_keyframe,
            per_scale_keyframe,
            per_animation_channel,
            per_animation,

            per_scene
        };

        extern const std::unordered_map<code, code> parents;
        extern const std::unordered_map<code, std::string> names;
        extern const std::unordered_map<code, std::string> patterns;
	}
}