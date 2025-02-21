#include "vtype.h"
#include "../max-bone-influence.h"

const std::unordered_map<std::string, mc::vtype::code> mc::vtype::codes = {
    { "i", indice},
    { "indice", indice},

    { "v", vertex},
    { "vert", vertex},
    { "vertex", vertex},
    { "n", normal},
    { "normal", normal},
    { "tc", uv},
    { "tex_coord", uv},
    { "texture_coordinate", uv},
    { "uv", uv },
    { "t", tangent },
    { "tangent", tangent },
    { "b", bitangent },
    { "bitangent", bitangent},
    { "vertex_color", vertex_color },
    { "bone_id", bone_id },
    { "bone_weight", bone_weight },

    { "m_off_matr", mesh_bone_offset_matrix },
    { "m_off_matrix", mesh_bone_offset_matrix },
    { "m_offset_matr", mesh_bone_offset_matrix },
    { "m_offset_matrix", mesh_bone_offset_matrix },
    { "m_bone_parent", mesh_bone_parent },
    { "m_bone_child", mesh_bone_child },

    { "off_matr", offset_matrix },
    { "off_matrix", offset_matrix },
    { "offset_matr", offset_matrix },
    { "offset_matrix", offset_matrix },
    { "bone_parent", bone_parent },
    { "bone_child", bone_child },

    { "position_key", position_key },
    { "rotation_key", rotation_key },
    { "scale_key", scale_key },
    { "position_key_timestamp", position_key_timestamp },
    { "rotation_key_timestamp", rotation_key_timestamp },
    { "scale_key_timestamp", scale_key_timestamp },
    { "position_key_time", position_key_timestamp },
    { "rotation_key_time", rotation_key_timestamp },
    { "scale_key_time", scale_key_timestamp },
    { "position_timestamp", position_key_timestamp },
    { "rotation_timestamp", rotation_key_timestamp },
    { "scale_timestamp", scale_key_timestamp },
    { "position_time", position_key_timestamp },
    { "rotation_time", rotation_key_timestamp },
    { "scale_time", scale_key_timestamp },

    { "duration", duration },
    { "ticks_per_second", ticks_per_second }
};

const std::unordered_map<mc::vtype::code, std::string> mc::vtype::names = {
    { null, "null"},

    { indice, "indice" },
    { vertex, "vertex"},
    { normal, "normal"},
    { tangent, "tangent"},
    { bitangent, "bitangent"},
    { uv, "uv" },
    { vertex_color, "vertex_color"},
    { bone_id, "bone_id"},
    { bone_weight, "bone_weight"},

    { mesh_bone_offset_matrix, "offset_matrix"},
    { mesh_bone_parent, "bone_parent" },
    { mesh_bone_child, "bone_child" },

    { offset_matrix, "offset_matrix"},
    { bone_parent, "bone_parent" },
    { bone_child, "bone_child" },

    { position_key, "position_key"},
    { rotation_key, "rotation_key"},
    { scale_key, "scale_key"},
    { position_key_timestamp, "position_key_timestamp"},
    { rotation_key_timestamp, "rotation_key_timestamp"},
    { scale_key_timestamp, "scale_key_timestamp"},

    { duration, "duration"},
    { ticks_per_second, "ticks_per_second"},
};

const std::unordered_map<mc::vtype::code, std::vector<unsigned short>> mc::vtype::maxSuffixes = {

    {position_key_timestamp, std::vector<unsigned short>()},
    {rotation_key_timestamp, std::vector<unsigned short>()},
    {scale_key_timestamp, std::vector<unsigned short>()},
    {duration, std::vector<unsigned short>()},
    {ticks_per_second, std::vector<unsigned short>()},
    {bone_parent, std::vector<unsigned short>()},
    {bone_child, std::vector<unsigned short>()},
    {mesh_bone_parent, std::vector<unsigned short>()},
    {mesh_bone_child, std::vector<unsigned short>()},

    {indice, std::vector<unsigned short>({3})},
    {vertex, std::vector<unsigned short>({3})},
    {normal, std::vector<unsigned short>({3})},
    {tangent, std::vector<unsigned short>({3})},
    {bitangent, std::vector<unsigned short>({3})},
    {position_key, std::vector<unsigned short>({3})},
    {scale_key, std::vector<unsigned short>({3})},

    {rotation_key, std::vector<unsigned short>({4})},

    {bone_id, std::vector<unsigned short>({MAX_BONE_INFLUENCE})},
    {bone_weight, std::vector<unsigned short>({MAX_BONE_INFLUENCE})},

    {uv, std::vector<unsigned short>({8, 3})},

    {vertex_color, std::vector<unsigned short>({8, 4})},

    {offset_matrix, std::vector<unsigned short>({4, 4})},
    {mesh_bone_offset_matrix, std::vector<unsigned short>({4, 4})}
};

const std::unordered_map<mc::vtype::code, mc::stype::code> mc::vtype::default_stypes = {
    {indice, stype::uint4},

    {bone_id, stype::int4},
    {bone_parent, stype::int4},
    {bone_child, stype::int4},
    {mesh_bone_parent, stype::int4},
    {mesh_bone_child, stype::int4},

    {vertex, stype::float4},
    {normal, stype::float4},
    {tangent, stype::float4},
    {bitangent, stype::float4},
    {uv, stype::float4},
    {vertex_color, stype::float4},
    {bone_weight, stype::float4},
    {position_key, stype::float4},
    {rotation_key, stype::float4},
    {scale_key, stype::float4},
    {offset_matrix, stype::float4},
    {mesh_bone_offset_matrix, stype::float4},

    {position_key_timestamp, stype::float8},
    {rotation_key_timestamp, stype::float8},
    {scale_key_timestamp, stype::float8},
    {duration, stype::float8},
    {ticks_per_second, stype::float8}
};

const std::unordered_map<mc::vtype::code, mc::ctype::code> mc::vtype::ctypes = {
    {indice, ctype::per_indice},

    {vertex, ctype::per_vertex},
    {normal, ctype::per_vertex},
    {tangent, ctype::per_vertex},
    {bitangent, ctype::per_vertex},
    {uv, ctype::per_vertex},
    {vertex_color, ctype::per_vertex},
    {bone_id, ctype::per_vertex},
    {bone_weight, ctype::per_vertex},

    {mesh_bone_offset_matrix, ctype::per_mesh_bone},
    {mesh_bone_parent, ctype::per_mesh_bone},
    {mesh_bone_child, ctype::per_mesh_bone_child},

    {offset_matrix, ctype::per_bone},
    {bone_parent, ctype::per_bone},
    {bone_child, ctype::per_bone_child},

    {position_key, ctype::per_position_keyframe},
    {position_key_timestamp, ctype::per_position_keyframe},

    {rotation_key, ctype::per_rotation_keyframe},
    {rotation_key_timestamp, ctype::per_rotation_keyframe},

    {scale_key, ctype::per_scale_keyframe},
    {scale_key_timestamp, ctype::per_scale_keyframe},

    {duration, ctype::per_animation_channel},
    {ticks_per_second, ctype::per_animation_channel}
};