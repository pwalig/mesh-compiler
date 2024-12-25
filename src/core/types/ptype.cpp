#include "ptype.h"

const std::unordered_map<std::string, mc::ptype::code> mc::ptype::codes = {
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

const std::unordered_map<mc::ptype::code, std::string> mc::ptype::names = {
    { null, "null"},
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

const std::unordered_map<mc::ptype::code, mc::stype::code> mc::ptype::default_stypes = {
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