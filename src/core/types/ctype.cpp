#include "ctype.h"

const std::unordered_map<mc::ctype::code, mc::ctype::code> mc::ctype::parents = {
    {per_indice, per_mesh},
    {per_vertex, per_mesh},
    {per_mesh_bone, per_mesh},
    {per_mesh_bone_child, per_mesh_bone},

    {per_bone, per_skeleton},
    {per_bone_child, per_bone},

    {per_position_keyframe, per_animation_channel},
    {per_rotation_keyframe, per_animation_channel },
    {per_scale_keyframe, per_animation_channel },

    {per_animation_channel, per_animation},

    {per_mesh, per_scene},
    {per_skeleton, per_scene },
    {per_animation, per_scene },
};

const std::unordered_map<mc::ctype::code, std::string> mc::ctype::names{
    {null, "null"},
    {per_indice, "per_indice"},
    {per_vertex, "per_vertex"},
    {per_mesh_bone, "per_mesh_bone"},
    {per_mesh_bone_child, "per_mesh_bone_child"},
    {per_mesh, "per_mesh"},
    {per_bone, "per_bone"},
    {per_bone_child, "per_bone_child"},
    {per_skeleton, "per_skeleton"},
    {per_position_keyframe, "per_position_keyframe"},
    {per_rotation_keyframe, "per_rotation_keyframe"},
    {per_scale_keyframe, "per_scale_keyframe"},
    {per_animation_channel, "per_animation_channel"},
    {per_animation, "per_animation"},
    {per_scene, "per_scene"}
};

const std::unordered_map<mc::ctype::code, std::string> mc::ctype::patterns{
    {per_indice, "{indice}"},
    {per_vertex, "{vertex}"},
    {per_mesh_bone, "{mesh_bone}"},
    {per_mesh, "{mesh}"},
    {per_bone, "{bone}"},
    {per_skeleton, "{skeleton}"},
    {per_position_keyframe, "{position_keyframe}"},
    {per_rotation_keyframe, "{rotation_keyframe}"},
    {per_scale_keyframe, "{scale_keyframe}"},
    {per_animation_channel, "{animation_channel}"},
    {per_animation, "{animation}"},
    {per_scene, "{scene}"}
};