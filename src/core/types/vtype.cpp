#include "vtype.h"
#include "../max-bone-influence.h"

const std::unordered_map<std::string, mc::vtype::code> mc::vtype::preambleCodes = {
    {"buffu", buffers_per_unit },
    {"buffs", buffer_size },
    {"entryu", entries_per_unit },
    {"entryb", entries_per_buffer },
    {"entrys", entry_size },
    {"fieldu", fields_per_unit },
    {"fieldb", fields_per_buffer },
    {"fielde", fields_per_entry },
    {"fields", field_size }
};

const std::unordered_map<std::string, mc::vtype::code> mc::vtype::fieldCodes = {
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

    { "off_matr", offset_matrix },
    { "off_matrix", offset_matrix },
    { "offset_matr", offset_matrix },
    { "offset_matrix", offset_matrix },

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

    { constant, "const"},
    { other_unit, "other_unit"},

    { indice, "indice" },
    { vertex, "vertex"},
    { normal, "normal"},
    { tangent, "tangent"},
    { bitangent, "bitangent"},
    { uv, "uv" },
    { vertex_color, "vertex_color"},
    { bone_id, "bone_id"},
    { bone_weight, "bone_weight"},

    { offset_matrix, "offset_matrix"},

    { position_key, "position_key"},
    { rotation_key, "rotation_key"},
    { scale_key, "scale_key"},
    { position_key_timestamp, "position_key_timestamp"},
    { rotation_key_timestamp, "rotation_key_timestamp"},
    { scale_key_timestamp, "scale_key_timestamp"},

    { duration, "duration"},
    { ticks_per_second, "ticks_per_second"},

    { unit_size, "units" },
    { buffer_size, "buffs" },
    { buffers_per_unit, "buffu" },
    { entry_size, "entrys" },
    { entries_per_unit, "entryu" },
    { entries_per_buffer, "entryb" },
    { field_size, "fields"},
    { fields_per_unit, "fieldu" },
    { fields_per_buffer, "fieldb"},
    { fields_per_entry, "fielde" }
};

const std::unordered_map<mc::vtype::code, std::vector<unsigned int>> mc::vtype::maxSuffixes = {

    {position_key_timestamp, std::vector<unsigned int>()},
    {rotation_key_timestamp, std::vector<unsigned int>()},
    {scale_key_timestamp, std::vector<unsigned int>()},
    {duration, std::vector<unsigned int>()},
    {ticks_per_second, std::vector<unsigned int>()},

    {indice, std::vector<unsigned int>({3})},
    {vertex, std::vector<unsigned int>({3})},
    {normal, std::vector<unsigned int>({3})},
    {tangent, std::vector<unsigned int>({3})},
    {bitangent, std::vector<unsigned int>({3})},
    {position_key, std::vector<unsigned int>({3})},
    {scale_key, std::vector<unsigned int>({3})},

    {rotation_key, std::vector<unsigned int>({4})},

    {bone_id, std::vector<unsigned int>({MAX_BONE_INFLUENCE})},
    {bone_weight, std::vector<unsigned int>({MAX_BONE_INFLUENCE})},

    {uv, std::vector<unsigned int>({8, 3})},

    {vertex_color, std::vector<unsigned int>({8, 4})},

    {offset_matrix, std::vector<unsigned int>({4, 4})}
};

const std::unordered_map<mc::vtype::code, mc::stype::code> mc::vtype::default_stypes = {
    {indice, stype::uint4},

    {bone_id, stype::int4},

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

    {position_key_timestamp, stype::float8},
    {rotation_key_timestamp, stype::float8},
    {scale_key_timestamp, stype::float8},
    {duration, stype::float8},
    {ticks_per_second, stype::float8},

    {unit_size, stype::uint4},
    {buffer_size, stype::uint4},
    {buffers_per_unit, stype::uint4},
    {entry_size, stype::uint4},
    {entries_per_unit, stype::uint4},
    {entries_per_buffer, stype::uint4},
    {field_size, stype::uint4},
    {fields_per_unit, stype::uint4},
    {fields_per_entry, stype::uint4},
    {fields_per_buffer, stype::uint4}
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

    {offset_matrix, ctype::per_bone},

    {position_key, ctype::per_position_keyframe},
    {position_key_timestamp, ctype::per_position_keyframe},

    {rotation_key, ctype::per_rotation_keyframe},
    {rotation_key_timestamp, ctype::per_rotation_keyframe},

    {scale_key, ctype::per_scale_keyframe},
    {scale_key_timestamp, ctype::per_scale_keyframe},

    {duration, ctype::per_animation_channel},
    {ticks_per_second, ctype::per_animation_channel},

    {constant, ctype::null}
};