#include "meshCompiler.h"
#include <iostream>
#include <sstream>
#include <map>
#include <assimpReader.h>
#include <NotImplemented.h>

std::string mesh_compiler::version = "v2.0.0";

// ========== DEFINES AND MAPS ==========

std::map<std::string, mesh_compiler::value> mesh_compiler::preambleMap = {
    {"buffu", value::buffers_per_unit },
    {"buffs", value::buffer_size },
    {"entryu", value::entries_per_unit },
    {"entryb", value::entries_per_buffer },
    {"entrys", value::entry_size },
    {"fieldu", value::fields_per_unit },
    {"fieldb", value::fields_per_buffer },
    {"fielde", value::fields_per_entry },
    {"fields", value::field_size },
};

std::map<std::string, mesh_compiler::value> mesh_compiler::fieldsMap = {
    { "i", value::indice},
    { "indice", value::indice},

    { "v", value::vertex},
    { "vert", value::vertex},
    { "vertex", value::vertex},
    { "n", value::normal},
    { "normal", value::normal},
    { "tc", value::uv},
    { "tex_coord", value::uv},
    { "texture_coordinate", value::uv},
    { "uv", value::uv },
    { "t", value::tangent },
    { "tangent", value::tangent },
    { "b", value::bitangent },
    { "bitangent", value::bitangent},
    { "vertex_color", value::vertex_color },
    { "bone_id", value::bone_id },
    { "bone_weight", value::bone_weight },

    { "off_matr", value::offset_matrix },
    { "off_matrix", value::offset_matrix },
    { "offset_matr", value::offset_matrix },
    { "offset_matrix", value::offset_matrix },

    { "position_key", value::position_key },
    { "rotation_key", value::rotation_key },
    { "scale_key", value::scale_key },
    { "position_key_timestamp", value::position_key_timestamp },
    { "rotation_key_timestamp", value::rotation_key_timestamp },
    { "scale_key_timestamp", value::scale_key_timestamp },
    { "position_key_time", value::position_key_timestamp },
    { "rotation_key_time", value::rotation_key_timestamp },
    { "scale_key_time", value::scale_key_timestamp },
    { "position_timestamp", value::position_key_timestamp },
    { "rotation_timestamp", value::rotation_key_timestamp },
    { "scale_timestamp", value::scale_key_timestamp },
    { "position_time", value::position_key_timestamp },
    { "rotation_time", value::rotation_key_timestamp },
    { "scale_time", value::scale_key_timestamp },

    { "duration", value::duration },
    { "ticks_per_second", value::ticks_per_second }
};

std::map<std::string, mesh_compiler::type> mesh_compiler::typesMap = {
    {"char", mc_char},
    {"short", mc_short},
    {"int", mc_int},
    {"long", mc_long},
    {"long_long", mc_long_long},
    {"int2", mc_short},
    {"int4", mc_int},
    {"int8", mc_long},
    {"int16", mc_long_long},
    {"unsigned_short", mc_unsigned_short},
    {"unsigned_int", mc_unsigned_int},
    {"unsigned_long", mc_unsigned_long},
    {"unsigned_long_long", mc_unsigned_long_long},
    {"unsigned_int2", mc_unsigned_short},
    {"unsigned_int4", mc_unsigned_int},
    {"unsigned_int8", mc_unsigned_long},
    {"unsigned_int16", mc_unsigned_long_long},
    {"ushort", mc_unsigned_short},
    {"uint", mc_unsigned_int},
    {"ulong", mc_unsigned_long},
    {"uint2", mc_unsigned_short},
    {"uint4", mc_unsigned_int},
    {"uint8", mc_unsigned_long},
    {"uint16", mc_unsigned_long_long},
    {"float", mc_float},
    {"float4", mc_float},
    {"double", mc_double},
    {"float8", mc_double},
    {"long_double", mc_long_double},
    {"float16", mc_long_double}
};

std::map<mesh_compiler::type, unsigned short> mesh_compiler::typeSizesMap = {
    {mc_char, sizeof(char)},
    {mc_short, sizeof(short)},
    {mc_int, sizeof(int)},
    {mc_long, sizeof(long)},
    {mc_long_long, sizeof(long long)},
    {mc_unsigned_short, sizeof(unsigned short)},
    {mc_unsigned_int, sizeof(unsigned int)},
    {mc_unsigned_long, sizeof(unsigned long)},
    {mc_unsigned_long_long, sizeof(unsigned long long)},
    {mc_float, sizeof(float)},
    {mc_double, sizeof(double)},
    {mc_long_double, sizeof(long double)}
};

std::map<mesh_compiler::type, std::string> mesh_compiler::typeNamesMap = {
    {mc_none, "null"},
    {mc_unit, "unit"},
    {mc_char, "char"},
    {mc_short, "int2"},
    {mc_int, "int4"},
    {mc_long, "int8"},
    {mc_long_long, "int16"},
    {mc_unsigned_short, "uint2"},
    {mc_unsigned_int, "uint4"},
    {mc_unsigned_long, "uint8"},
    {mc_unsigned_long_long, "uint16"},
    {mc_float, "float4"},
    {mc_double, "float8"},
    {mc_long_double, "float16"}
};
std::map<mesh_compiler::value, std::string> mesh_compiler::valueNamesMap = {
    { value::null, "null"},

    { value::constant, "const"},
    { value::other_unit, "other_unit"},

    { value::indice, "indice" },
    { value::vertex, "vertex"},
    { value::normal, "normal"},
    { value::tangent, "tangent"},
    { value::bitangent, "bitangent"},
    { value::uv, "uv" },
    { value::vertex_color, "vertex_color"},
    { value::bone_id, "bone_id"},
    { value::bone_weight, "bone_weight"},

    { value::offset_matrix, "offset_matrix"},

    { value::position_key, "position_key"},
    { value::rotation_key, "rotation_key"},
    { value::scale_key, "scale_key"},
    { value::position_key_timestamp, "position_key_timestamp"},
    { value::rotation_key_timestamp, "rotation_key_timestamp"},
    { value::scale_key_timestamp, "scale_key_timestamp"},

    { value::duration, "duration"},
    { value::ticks_per_second, "ticks_per_second"},

    { value::unit_size, "units" },
    { value::buffer_size, "buffs" },
    { value::buffers_per_unit, "buffu" },
    { value::entry_size, "entrys" },
    { value::entries_per_unit, "entryu" },
    { value::entries_per_buffer, "entryb" },
    { value::field_size, "fields"},
    { value::fields_per_unit, "fieldu" },
    { value::fields_per_buffer, "fieldb"},
    { value::fields_per_entry, "fielde" }
};
std::map<mesh_compiler::counting_type, std::string> mesh_compiler::countingTypeNamesMap = {
    {counting_type::null, "null"},
    {counting_type::per_indice, "per_indice"},
    {counting_type::per_vertex, "per_vertex"},
    {counting_type::per_mesh_bone, "per_mesh_bone"},
    {counting_type::per_mesh, "per_mesh"},
    {counting_type::per_bone, "per_bone"},
    {counting_type::per_skeleton, "per_skeleton"},
    {counting_type::per_position_keyframe, "per_position_keyframe"},
    {counting_type::per_rotation_keyframe, "per_rotation_keyframe"},
    {counting_type::per_scale_keyframe, "per_scale_keyframe"},
    {counting_type::per_animation_channel, "per_animation_channel"},
    {counting_type::per_animation, "per_animation"},
};

std::map<char, unsigned short> suffixesMap = {
    {'0', 0}, {'1', 1}, {'2', 2}, {'3', 3}, {'4', 4}, {'5', 5}, {'6', 6}, {'7', 7},
    {'x', 0}, {'y', 1}, {'z', 2},
    {'r', 0}, {'g', 1}, {'b', 2}, {'a', 3},
    {'u', 0}, {'v', 1}, {'w', 2}
};

mesh_compiler::type mesh_compiler::getDefaultValueType(const value& v)
{
    switch (v)
    {
    case value::indice:
        return mc_unsigned_int;

    case value::bone_id:
        return mc_int;

    case value::vertex:
    case value::normal:
    case value::tangent:
    case value::bitangent:
    case value::uv:
    case value::vertex_color:
    case value::bone_weight:
    case value::position_key:
    case value::rotation_key:
    case value::scale_key:
    case value::offset_matrix:
        return mc_float;

    case value::position_key_timestamp:
    case value::rotation_key_timestamp:
    case value::scale_key_timestamp:
    case value::duration:
    case value::ticks_per_second:
        return mc_double;

    case value::unit_size:
    case value::buffer_size:
    case value::buffers_per_unit:
    case value::entry_size:
    case value::entries_per_unit:
    case value::entries_per_buffer:
    case value::field_size:
    case value::fields_per_unit:
    case value::fields_per_entry:
    case value::fields_per_buffer:
        return mc_unsigned_int;

    default:
        throw std::logic_error("value type without default type");
    }
}

mesh_compiler::counting_type mesh_compiler::getFieldCount(const value& t) {
    switch (t)
    {
    case value::indice:
        return counting_type::per_indice;

    case value::vertex:
    case value::normal:
    case value::tangent:
    case value::bitangent:
    case value::uv:
    case value::vertex_color:
    case value::bone_id:
    case value::bone_weight:
        return counting_type::per_vertex;

    case value::offset_matrix:
        return counting_type::per_bone;

    case value::position_key:
    case value::position_key_timestamp:
        return counting_type::per_position_keyframe;

    case value::rotation_key:
    case value::rotation_key_timestamp:
        return counting_type::per_rotation_keyframe;

    case value::scale_key:
    case value::scale_key_timestamp:
        return counting_type::per_scale_keyframe;

    case value::duration:
    case value::ticks_per_second:
        return counting_type::per_animation_channel;

    case value::constant:
        return counting_type::null;

    default:
        throw std::logic_error("value with no field count querried for field count");
    }
}

mesh_compiler::counting_type mesh_compiler::getParentCountingType(const counting_type& ct)
{
    switch (ct)
    {
    case counting_type::per_indice:
    case counting_type::per_vertex:
    case counting_type::per_mesh_bone:
        return counting_type::per_mesh;

    case counting_type::per_bone:
        return counting_type::per_skeleton;

    case counting_type::per_position_keyframe:
    case counting_type::per_rotation_keyframe:
    case counting_type::per_scale_keyframe:
        return counting_type::per_animation_channel;

    case counting_type::per_animation_channel:
        return counting_type::per_animation;

    case counting_type::per_mesh:
    case counting_type::per_skeleton:
    case counting_type::per_animation:
        return counting_type::per_scene;

    default:
        throw std::logic_error("counting type has no parent");
    }
}

std::vector<unsigned short> mesh_compiler::getMaxSuffixes(const value& t)
{
    std::vector<unsigned short> out;
    switch (t)
    {
    case value::position_key_timestamp:
    case value::rotation_key_timestamp:
    case value::scale_key_timestamp:
    case value::duration:
    case value::ticks_per_second:
        return out;
    case value::indice:
    case value::vertex:
    case value::normal:
    case value::tangent:
    case value::bitangent:
    case value::position_key:
    case value::scale_key:
        out.push_back(3);
        return out;
    case value::rotation_key:
        out.push_back(4);
        return out;
    case value::bone_id:
    case value::bone_weight:
        out.push_back(MAX_BONE_INFLUENCE);
        return out;
    case value::uv:
        out.push_back(8);
        out.push_back(3);
        return out;
    case value::vertex_color:
        out.push_back(8);
        out.push_back(4);
        return out;
    case value::offset_matrix:
        out.push_back(4);
        out.push_back(4);
        return out;
    default:
        throw std::logic_error("value with no allowed suffixes querried for max suffixes");
        break;
    }
}

void mesh_compiler::copyConstantToMemory(void* dst, const type& t, const std::string& val) {
    union data_union {
        char c;
        short s;
        int i;
        long l;
        long long ll;
        float f;
        double d;
        long double ld;
    };

    data_union data;

    switch (t)
    {
    case mc_char:
        if (val.size() > 1)
            throw formatInterpreterException(formatInterpreterException::error_code::invalid_const_value, "char value must be only one character, given: " + std::to_string(val.size()));
        data.c = val[0];
        break;
    case mc_short:
    case mc_unsigned_short:
        try {
            data.s = std::stoi(val);
        }
        catch (std::exception& e) {
            throw formatInterpreterException(formatInterpreterException::error_code::invalid_const_value);
        }
        break;
    case mc_int:
    case mc_unsigned_int:
        try {
            data.i = std::stoi(val);
        }
        catch (std::exception& e) {
            throw formatInterpreterException(formatInterpreterException::error_code::invalid_const_value);
        }
        break;
    case mc_long:
    case mc_unsigned_long:
        try {
            data.l = std::stol(val);
        }
        catch (std::exception& e) {
            throw formatInterpreterException(formatInterpreterException::error_code::invalid_const_value);
        }
        break;
    case mc_long_long:
    case mc_unsigned_long_long:
        try {
            data.ll = std::stoll(val);
        }
        catch (std::exception& e) {
            throw formatInterpreterException(formatInterpreterException::error_code::invalid_const_value);
        }
        break;
    case mc_float:
        try {
            data.f = std::stof(val);
        }
        catch (std::exception& e) {
            throw formatInterpreterException(formatInterpreterException::error_code::invalid_const_value);
        }
        break;
    case mc_double:
        try {
            data.d = std::stod(val);
        }
        catch (std::exception& e) {
            throw formatInterpreterException(formatInterpreterException::error_code::invalid_const_value);
        }
        break;
    case mc_long_double:
        try {
            data.ld = std::stold(val);
        }
        catch (std::exception& e) {
            throw formatInterpreterException(formatInterpreterException::error_code::invalid_const_value);
        }
        break;
    default:
        throw formatInterpreterException(formatInterpreterException::error_code::invalid_const_value);
        break;
    }

    switch (t) {
    case mc_unsigned_short:
        if (data.s < 0) throw formatInterpreterException(formatInterpreterException::error_code::invalid_const_value, "negative value passed as unsigned type");
        break;
    case mc_unsigned_int:
        if (data.i < 0) throw formatInterpreterException(formatInterpreterException::error_code::invalid_const_value, "negative value passed as unsigned type");
        break;
    case mc_unsigned_long:
        if (data.l < 0) throw formatInterpreterException(formatInterpreterException::error_code::invalid_const_value, "negative value passed as unsigned type");
        break;
    case mc_unsigned_long_long:
        if (data.ll < 0) throw formatInterpreterException(formatInterpreterException::error_code::invalid_const_value, "negative value passed as unsigned type");
        break;
    default:
        break;
    }
    memcpy(dst, &data, typeSizesMap[t]);
}

// ========== EXCEPTIONS ==========

std::map<mesh_compiler::formatInterpreterException::error_code, std::string> mesh_compiler::formatInterpreterException::errorMessagesMap = {
    {formatInterpreterException::error_code::unknown_statement, "unknown statement"},
    {formatInterpreterException::error_code::no_suffix, "field suffix not provided"},
    {formatInterpreterException::error_code::invalid_suffix, "invalid field suffix"},
    {formatInterpreterException::error_code::wrong_suffixes_amount, "wrong amount of suffixes"},
    {formatInterpreterException::error_code::no_const_value, "constant value not provided"},
    {formatInterpreterException::error_code::invalid_const_value, "invalid const value"},
    {formatInterpreterException::error_code::invalid_type_specifier, "invalid type specifier"},
    {formatInterpreterException::error_code::byte_base_in_count_type, "given byte base in count type"},
    {formatInterpreterException::error_code::field_spec_in_preamble, "attempted field specification in preamble"},
    {formatInterpreterException::error_code::unsupported_type, "unsupported type"},
    {formatInterpreterException::error_code::conflicting_buffer_fields, "confilcting types detected in single buffer"},
    {formatInterpreterException::error_code::conflicting_unit_fields, "confilcting types detected in single unit"},
    {formatInterpreterException::error_code::constants_only, "detected buffer of only constants - unknown buffer size"},
    {formatInterpreterException::error_code::no_unit_name, "unit name unspecified"},
    {formatInterpreterException::error_code::no_file_name, "file name unspecified"},
    {formatInterpreterException::error_code::unit_redefinition, "unit redefinition"},
    {formatInterpreterException::error_code::name_keyword_collision, "unit name collides with keyword"},
    {formatInterpreterException::error_code::no_end, "end key word expected"},
    {formatInterpreterException::error_code::unknown, "unknown error"}
};

mesh_compiler::formatInterpreterException::formatInterpreterException(const error_code& error_code, const std::string& message) : type(error_code), msg(message), filled(false)
{
    if (errorMessagesMap.find(type) == errorMessagesMap.end()) type = formatInterpreterException::error_code::unknown;
}

mesh_compiler::formatInterpreterException::formatInterpreterException(const error_code& error_code, const unsigned int& line_number, const std::string& processed_word, const std::string& message) : formatInterpreterException(error_code, message)
{
    fillInfo(line_number, processed_word);
}

std::string mesh_compiler::formatInterpreterException::make_message(const error_code& error_code, const unsigned int& line_number, const std::string& processed_word, const std::string& message)
{
    return "format compilation error: " + errorMessagesMap[error_code] + ": " + processed_word + " in line " + std::to_string(line_number) + ". " + message;
}

void mesh_compiler::formatInterpreterException::fillInfo(const unsigned int& line_number, const std::string& processed_word)
{
    this->msg = "format compilation error: " + errorMessagesMap[type] + ": " + processed_word + " in line " + std::to_string(line_number) + ". " + this->msg;
    this->filled = true;
}

const char* mesh_compiler::formatInterpreterException::what() throw()
{
    if (!filled) this->msg = "format compilation error: " + errorMessagesMap[type] + ": info not filled";
    return this->msg.c_str();
}

mesh_compiler::meshCompilerException::meshCompilerException(const std::string& message) : msg(message)
{
}

const char* mesh_compiler::meshCompilerException::what() throw()
{
    return msg.c_str();
}

// ========== METHODS DEFINITIONS ==========

mesh_compiler::compileField::compileField(const type& s, const value& v, const void* data_source) : compileField(s, v, data_source, typeSizesMap[s])
{
}

mesh_compiler::compileField::compileField(const type& s, const value& v, const void* data_source, const size_t& data_amount) : stype(s), vtype(v)
{
    if (s == mc_none) throw std::logic_error("unable to construct compile Field of type: none");
    if (v == value::null) throw std::logic_error("unable to construct compile Field of value: null");
    if (data_source != nullptr) {
        setData(data_source, data_amount);
    }
}

void mesh_compiler::compileField::setAsConst(const type& t, const void* data_source)
{
    vtype = value::constant;
    setData(data_source, typeSizesMap[t]);
}

void mesh_compiler::compileField::setData(const void* data_source, const size_t& data_amount)
{
    data.resize(data_amount);
    memcpy(data.data(), data_source, data_amount);
}

size_t mesh_compiler::compileField::get_size() const
{
    return typeSizesMap[stype];
}

std::string mesh_compiler::compileField::get_otherUnitName() const
{
    if (this->vtype != value::other_unit) throw std::logic_error("attempted to get unit name when value type was not other unit");
    return std::string(this->data.begin(), this->data.end());
}

void mesh_compiler::compileField::print(const int& indent) const
{
    for (int i = 0; i < indent; ++i) printf(" ");
    std::cout << "FIELD: " << typeNamesMap[stype] << ":" << valueNamesMap[vtype] << ":";
    for (const char& c : data) std::cout << c;
}

void mesh_compiler::compileField::put(std::ofstream& file, const compileBuffer& buffer) const
{
    // size
    switch (this->vtype)
    {
    case value::constant:
        file.write(this->data.data(), typeSizesMap[this->stype]);
        break;
    case value::buffer_size:
        writeConst(file, buffer.get_size(), this->stype);
        break;
    case value::entry_size:
        writeConst(file, buffer.get_entry_size(), this->stype);
        break;
    case value::entries_per_buffer:
        writeConst(file, buffer.count, this->stype);
        break;
    case value::field_size:
        for (const compileField& cf : buffer.fields) {
            writeConst(file, cf.get_size(), this->stype);
        }
        break;
    case value::fields_per_entry:
        writeConst(file, buffer.fields.size(), this->stype);
        break;
    case value::fields_per_buffer:
        writeConst(file, buffer.fields.size() * buffer.count, this->stype);
        break;
    default:
        throw std::logic_error("flag could not be handled with this function call, flag: " + valueNamesMap[this->vtype]);
        break;
    }
}

void mesh_compiler::compileField::put(std::ofstream& file, const std::vector<compileBuffer>& buffers) const
{
    switch (this->vtype)
    {
    case value::constant:
        file.write(this->data.data(), typeSizesMap[this->stype]);
        break;
    case value::buffer_size:
    case value::entry_size:
    case value::entries_per_buffer:
    case value::field_size:
    case value::fields_per_entry:
    case value::fields_per_buffer:
        for (const compileBuffer& cb : buffers) {
            this->put(file, cb);
        }
        break;
    default:
        throw std::logic_error("flag could not be handled with this function call, flag: " + valueNamesMap[this->vtype]);
        break;
    }
}

void mesh_compiler::compileField::put(std::ofstream& file, const compileUnit& unit) const
{
    switch (this->vtype)
    {
    case value::constant:
        file.write(this->data.data(), typeSizesMap[this->stype]);
        break;
    case value::buffers_per_unit:
        writeConst(file, unit.buffers.size(), this->stype);
        break;
    case value::entries_per_unit:
        writeConst(file, unit.get_entries_count(), this->stype);
        break;
    case value::fields_per_unit:
        writeConst(file, unit.get_fields_count(), this->stype);
        break;
    default:
        this->put(file, unit.buffers);
        break;
    }
}

bool mesh_compiler::compileField::operator==(const compileField& other) const
{
    return (this->vtype == other.vtype && this->stype == other.stype && this->data == other.data);
}

bool mesh_compiler::compileField::operator!=(const compileField& other) const
{
    return !(*this == other);
}

size_t mesh_compiler::compileBuffer::get_entry_size() const
{
    size_t siz = 0;
    for (const compileField& cf : fields)
        siz += cf.get_size();
    return siz;
}

size_t mesh_compiler::compileBuffer::get_size() const
{
    return get_entry_size() * count;
}

void mesh_compiler::compileBuffer::print(const int& indent) const
{
    for (int i = 0; i < indent; ++i) printf(" ");
    std::cout << "BUFFER: preamble: ";
    for (const compileField& f : preamble) {
        f.print();
    }
    std::cout << ", count: " << count << ", fields: \n";
    for (const compileField& f : fields) {
        f.print(indent + 2);
        std::cout << std::endl;
    }
}

void mesh_compiler::compileBuffer::clear()
{
    this->preamble.clear();
    this->fields.clear();
}

bool mesh_compiler::compileBuffer::operator==(const compileBuffer& other) const
{
    return (this->count_type == other.count_type && this->preamble == other.preamble && this->fields == other.fields);
}

bool mesh_compiler::compileBuffer::operator!=(const compileBuffer& other) const
{
    return !(*this == other);
}

size_t mesh_compiler::compileUnit::get_size() const
{
    size_t siz = 0;
    for (const compileBuffer& cb : buffers) siz += cb.get_size();
    return siz;
}

size_t mesh_compiler::compileUnit::get_entries_count() const
{
    size_t siz = 0;
    for (const compileBuffer& cb : buffers) siz += cb.count;
    return siz;
}

size_t mesh_compiler::compileUnit::get_fields_count() const
{
    size_t siz = 0;
    for (const compileBuffer& cb : buffers) siz += cb.fields.size();
    return siz;
}

void mesh_compiler::compileUnit::print(const int& indent) const
{
    for (int i = 0; i < indent; ++i) printf(" ");
    std::cout << "UNIT: preamble: ";
    for (const compileField& f : preamble) f.print();
    std::cout << "\n";
    for (const compileBuffer& b : buffers) b.print(indent + 2);
}

void mesh_compiler::compileUnit::clear()
{
    this->preamble.clear();
    this->buffers.clear();
}

void mesh_compiler::compileUnit::put(std::ofstream& file, const aiNodeAnim* animation_channel)
{
    if (this->count_type != counting_type::per_animation_channel) throw meshCompilerException("invalid compilation unit for this object");
    // fill counts
    for (compileBuffer& buffer : this->buffers) {
        if (buffer.count_type == counting_type::per_position_keyframe) buffer.count = animation_channel->mNumPositionKeys;
        else if (buffer.count_type == counting_type::per_rotation_keyframe) buffer.count = animation_channel->mNumRotationKeys;
        else if (buffer.count_type == counting_type::per_scale_keyframe) buffer.count = animation_channel->mNumScalingKeys;
        else {
            throw std::logic_error("invalid counting type for this animation object" + countingTypeNamesMap[buffer.count_type]);
        }
    }

    // preamble
    for (const compileField& field : this->preamble) {
        if (field.vtype == value::other_unit) (*unitsMap)[field.get_otherUnitName()].put(file, animation_channel);
        else field.put(file, *this);
    }

    // buffers
    for (const compileBuffer& buffer : this->buffers) {

        // buffer preamble
        for (const compileField& field : buffer.preamble) {
            if (field.vtype == value::other_unit) (*unitsMap)[field.get_otherUnitName()].put(file, animation_channel);
            else field.put(file, buffer);
        }

        // fields
        for (unsigned int j = 0; j < buffer.count; ++j) {
            for (const compileField& field : buffer.fields) {
                switch (field.vtype)
                {
                case value::constant:
                    file.write(field.data.data(), typeSizesMap[field.stype]);
                    break;
                case value::position_key:
                    writeConst(file, animation_channel->mPositionKeys[j].mValue[field.data[0]], field.stype);
                    break;
                case value::rotation_key:
                    switch (field.data[0]) {
                    case 0:
                        writeConst(file, animation_channel->mRotationKeys[j].mValue.x, field.stype);
                        break;
                    case 1:
                        writeConst(file, animation_channel->mRotationKeys[j].mValue.y, field.stype);
                        break;
                    case 2:
                        writeConst(file, animation_channel->mRotationKeys[j].mValue.z, field.stype);
                        break;
                    case 3:
                        writeConst(file, animation_channel->mRotationKeys[j].mValue.w, field.stype);
                        break;
                    }
                    break;
                case value::scale_key:
                    writeConst(file, animation_channel->mScalingKeys[j].mValue[field.data[0]], field.stype);
                    break;
                case value::position_key_timestamp:
                    writeConst(file, animation_channel->mPositionKeys[j].mTime, field.stype);
                    break;
                case value::rotation_key_timestamp:
                    writeConst(file, animation_channel->mRotationKeys[j].mTime, field.stype);
                    break;
                case value::scale_key_timestamp:
                    writeConst(file, animation_channel->mScalingKeys[j].mTime, field.stype);
                    break;
                default:
                    throw std::logic_error("invalid value");
                    break;
                }
            }
        }
    }
}

void mesh_compiler::compileUnit::put(std::ofstream& file, const aiSkeleton* skeleton)
{
    if (this->count_type != counting_type::per_skeleton) throw meshCompilerException("invalid compilation unit for this object");
    // fill counts
    for (compileBuffer& buffer : this->buffers) {
        if (buffer.count_type == counting_type::per_bone) buffer.count = skeleton->mNumBones;
        else {
            throw std::logic_error("invalid counting type for scene object" + countingTypeNamesMap[buffer.count_type]);
        }
    }

    // preamble
    for (const compileField& field : this->preamble) {
        if (field.vtype == value::other_unit) (*unitsMap)[field.get_otherUnitName()].put(file, skeleton);
        else field.put(file, *this);
    }

    // buffers
    for (const compileBuffer& buffer : this->buffers) {

        // buffer preamble
        for (const compileField& field : buffer.preamble) {
            if (field.vtype == value::other_unit) (*unitsMap)[field.get_otherUnitName()].put(file, skeleton);
            else field.put(file, buffer);
        }

        // fields
        for (unsigned int j = 0; j < buffer.count; ++j) {
            for (const compileField& field : buffer.fields) {
                switch (field.vtype)
                {
                case value::constant:
                    file.write(field.data.data(), typeSizesMap[field.stype]);
                    break;
                case value::offset_matrix:
                    writeConst(file, skeleton->mBones[j]->mOffsetMatrix[field.data[0]][field.data[1]], field.stype);
                    break;
                default:
                    throw std::logic_error("invalid value");
                    break;
                }
            }
        }
    }
}

void mesh_compiler::compileUnit::put(std::ofstream& file, const aiAnimation* animation)
{
    if (this->count_type != counting_type::per_animation) throw meshCompilerException("invalid compilation unit for this object");
    // fill counts
    for (compileBuffer& buffer : this->buffers) {
        if (buffer.count_type == counting_type::per_animation_channel) buffer.count = animation->mNumChannels;
        else {
            throw std::logic_error("invalid counting type for this animation object" + countingTypeNamesMap[buffer.count_type]);
        }
    }

    // preamble
    for (const compileField& field : this->preamble) {
        if (field.vtype == value::other_unit) (*unitsMap)[field.get_otherUnitName()].put(file, animation);
        else field.put(file, *this);
    }

    // buffers
    for (const compileBuffer& buffer : this->buffers) {

        // buffer preamble
        for (const compileField& field : buffer.preamble) {
            if (field.vtype == value::other_unit) (*unitsMap)[field.get_otherUnitName()].put(file, animation);
            else field.put(file, buffer);
        }

        // fields
        for (unsigned int j = 0; j < buffer.count; ++j) {
            for (const compileField& field : buffer.fields) {
                switch (field.vtype)
                {
                case value::constant:
                    file.write(field.data.data(), typeSizesMap[field.stype]);
                    break;
                case value::other_unit:
                    (*unitsMap)[field.get_otherUnitName()].put(file, animation->mChannels[j]);
                    break;
                case value::duration:
                    writeConst(file, animation->mDuration, field.stype);
                    break;
                case value::ticks_per_second:
                    writeConst(file, animation->mTicksPerSecond, field.stype);
                    break;
                default:
                    throw std::logic_error("invalid value");
                    break;
                }
            }
        }
    }
}

void mesh_compiler::compileUnit::put(std::ofstream& file, const aiMesh* mesh)
{
    assimp::meshWeights<int, float, MAX_BONE_INFLUENCE> mw(mesh);
    put(file, mesh, mw);
}

void mesh_compiler::compileUnit::put(std::ofstream& file, const aiMesh* mesh, const assimp::meshWeights<int, float, MAX_BONE_INFLUENCE>& mw)
{
    if (this->count_type != counting_type::per_mesh) throw meshCompilerException("invalid compilation unit for this object");
    // fill counts
    for (compileBuffer& buffer : this->buffers) {
        if (buffer.count_type == counting_type::per_indice) buffer.count = mesh->mNumFaces;
        else if (buffer.count_type == counting_type::per_vertex) buffer.count = mesh->mNumVertices;
        else if (buffer.count_type == counting_type::per_mesh_bone) buffer.count = mesh->mNumBones;
        else {
            throw std::logic_error("invalid counting type for scene object" + countingTypeNamesMap[buffer.count_type]);
        }
    }

    // preamble
    for (const compileField& field : this->preamble) {
        if (field.vtype == value::other_unit) (*unitsMap)[field.get_otherUnitName()].put(file, mesh, mw);
        else field.put(file, *this);
    }

    // buffers
    for (const compileBuffer& buffer : this->buffers) {

        // buffer preamble
        for (const compileField& field : buffer.preamble) {
            if (field.vtype == value::other_unit) (*unitsMap)[field.get_otherUnitName()].put(file, mesh, mw);
            else field.put(file, buffer);
        }

        // fields
        for (unsigned int j = 0; j < buffer.count; ++j) {
            for (const compileField& field : buffer.fields) {
                switch (field.vtype)
                {
                case value::constant:
                    file.write(field.data.data(), typeSizesMap[field.stype]);
                    break;
                case value::indice:
                    writeConst(file, mesh->mFaces[j].mIndices[field.data[0]], field.stype);
                    break;
                case value::vertex:
                    writeConst(file, mesh->mVertices[j][field.data[0]], field.stype);
                    break;
                case value::normal:
                    writeConst(file, mesh->mNormals[j][field.data[0]], field.stype);
                    break;
                case value::uv:
                    writeConst(file, mesh->mTextureCoords[field.data[0]][j][field.data[1]], field.stype);
                    break;
                case value::tangent:
                    writeConst(file, mesh->mTangents[j][field.data[0]], field.stype);
                    break;
                case value::bitangent:
                    writeConst(file, mesh->mBitangents[j][field.data[0]], field.stype);
                    break;
                case value::vertex_color:
                    writeConst(file, mesh->mColors[field.data[0]][j][field.data[1]], field.stype);
                    break;
                case value::bone_id:
                    writeConst(file, mw.vertices[j].bone_ids[field.data[0]], field.stype);
                    break;
                case value::bone_weight:
                    writeConst(file, mw.vertices[j].weights[field.data[0]], field.stype);
                    break;
                default:
                    throw std::logic_error("invalid value");
                    break;
                }
            }
        }
    }
}

void mesh_compiler::compileUnit::put(std::ofstream& file, const aiScene* scene)
{
    if (this->count_type != counting_type::per_scene) throw meshCompilerException("invalid compilation unit for this object");

    // fill counts
    for (compileBuffer& buffer : this->buffers) {
        if (buffer.count_type == counting_type::per_mesh) buffer.count = scene->mNumMeshes;
        else if (buffer.count_type == counting_type::per_skeleton) buffer.count = scene->mNumSkeletons;
        else if (buffer.count_type == counting_type::per_animation) buffer.count = scene->mNumAnimations;
        else {
            throw std::logic_error("invalid counting type for this mesh" + countingTypeNamesMap[buffer.count_type]);
        }
    }

    // preamble
    for (const compileField& field : this->preamble) {
        if (field.vtype == value::other_unit) (*unitsMap)[field.get_otherUnitName()].put(file, scene);
        else field.put(file, *this);
    }

    // buffers
    for (const compileBuffer& buffer : this->buffers) {

        // buffer preamble
        for (const compileField& field : buffer.preamble) {
            if (field.vtype == value::other_unit) (*unitsMap)[field.get_otherUnitName()].put(file, scene);
            else field.put(file, buffer);
        }

        // fields
        for (unsigned int j = 0; j < buffer.count; ++j) {
            for (const compileField& field : buffer.fields) {
                switch (field.vtype)
                {
                case value::constant:
                    file.write(field.data.data(), typeSizesMap[field.stype]);
                    break;
                case value::other_unit:
                    switch (buffer.count_type) {
                    case counting_type::per_mesh:
                        (*unitsMap)[field.get_otherUnitName()].put(file, scene->mMeshes[j]);
                        break;
                    case counting_type::per_skeleton:
                        (*unitsMap)[field.get_otherUnitName()].put(file, scene->mSkeletons[j]);
                        break;
                    case counting_type::per_animation:
                        (*unitsMap)[field.get_otherUnitName()].put(file, scene->mAnimations[j]);
                        break;
                    }
                    break;
                default:
                    throw std::logic_error("value type");
                    break;
                }
            }
        }
    }
}

bool mesh_compiler::compileUnit::operator==(const compileUnit& other) const
{
    return (this->count_type == other.count_type && this->preamble == other.preamble && this->buffers == other.buffers);
}

bool mesh_compiler::compileUnit::operator!=(const compileUnit& other) const
{
    return !(*this == other);
}

mesh_compiler::type mesh_compiler::compileUnit::extractType(std::string& word)
{
    type t = mc_none;
    size_t pos = word.find(':');
    if (pos != std::string::npos) {
        if (typesMap.find(word.substr(0, pos)) == typesMap.end()) throw formatInterpreterException(formatInterpreterException::error_code::invalid_type_specifier);
        t = typesMap[word.substr(0, pos)];
        word = word.substr(pos + 1, word.size() - pos - 1);
    }

    if (word.empty()) {
        if (t != mc_none) throw formatInterpreterException(formatInterpreterException::error_code::no_const_value);
        else throw std::logic_error("type somehow ended up null despite empty word");
    }

    return t;
}

mesh_compiler::value mesh_compiler::compileUnit::extractPreambleValue(std::string& word)
{
    value v = value::null;
    if (preambleMap.find(word) != preambleMap.end()) {
        v = preambleMap[word];
        word = "";
    }
    return v;
}

mesh_compiler::value mesh_compiler::compileUnit::extractFieldValue(std::string& word)
{
    value v = value::null;
    size_t pos = word.find('.');
    std::string ftype = word;
    if (pos != std::string::npos) {
        ftype = word.substr(0, pos);
    }
    else pos = word.size();
    if (fieldsMap.find(ftype) != fieldsMap.end()) {
        v = fieldsMap[ftype];
        word = word.substr(pos, word.size() - pos);
    }
    return v;
}

bool mesh_compiler::compileUnit::isPreambleValue(type t, std::string& arg, std::vector<compileField>& fields)
{
    value v = extractPreambleValue(arg);
    if (v != value::null) {
        if (t == mc_none) t = getDefaultValueType(v);
        fields.push_back(compileField(t, v, nullptr, 0));
        return true;
    }
    return false;
}

bool mesh_compiler::compileUnit::isFieldValue(type t, std::string& arg, std::vector<compileField>& fields, counting_type& field_count, counting_type& unit_count)
{
    value v = extractFieldValue(arg);
    if (v != value::null) {
        if (t == mc_none) t = getDefaultValueType(v);
        compileField field(t, v, nullptr, 0);

        // field counting type
        counting_type ffc = getFieldCount(field.vtype);
        if (ffc != counting_type::null) {
            if (ffc != field_count && field_count != counting_type::null) {
                throw formatInterpreterException(formatInterpreterException::error_code::conflicting_buffer_fields, " conflicting types: " + valueNamesMap[field.vtype] + " and " + countingTypeNamesMap[field_count]);
            }
            field_count = ffc;
        }

        // unit counting type
        ffc = getParentCountingType(field_count);
        if (unit_count == counting_type::null) unit_count = ffc;
        else if (unit_count != ffc) throw formatInterpreterException(formatInterpreterException::error_code::conflicting_unit_fields, " conflicting types: " + countingTypeNamesMap[ffc] + " and " + countingTypeNamesMap[unit_count]);

        std::vector<unsigned short> suffixes_data = getMaxSuffixes(v);
        while (arg.size() > 0) {
            if (arg.size() == 1) {
                if (arg[0] == '.') throw formatInterpreterException(formatInterpreterException::error_code::no_suffix);
                else throw formatInterpreterException(formatInterpreterException::error_code::unknown_statement);
            }
            if (arg[0] != '.') throw formatInterpreterException(formatInterpreterException::error_code::unknown_statement);
            if (suffixesMap.find(arg[1]) == suffixesMap.end()) throw formatInterpreterException(formatInterpreterException::error_code::invalid_suffix);

            field.data.push_back(suffixesMap[arg[1]]);
            if (field.data.size() > suffixes_data.size()) throw formatInterpreterException(formatInterpreterException::error_code::wrong_suffixes_amount, "this field type requires up to " + std::to_string(suffixes_data.size()) + " suffixes");
            if (field.data.back() >= suffixes_data[field.data.size() - 1]) throw formatInterpreterException(formatInterpreterException::error_code::invalid_suffix, arg.substr(0, 2) + " suffix must be less than " + std::to_string(suffixes_data[field.data.size() - 1]));

            arg = arg.substr(2, arg.size() - 2);
        }

        if (field.data.size() == suffixes_data.size()) {
            fields.push_back(field);
            return true;
        }

        for (int j = field.data.size(); j < suffixes_data.size(); ++j) {
            field.data.push_back(0);
            for (int i = 0; i < suffixes_data[j]; ++i) {
                field.data.back() = i;
                fields.push_back(field);
            }
        }

        return true;
    }
    return false;
}

bool mesh_compiler::compileUnit::isConstValue(const type& t, std::string& arg, std::vector<compileField>& fields)
{
    if (t == mc_none) return false;

    fields.push_back(compileField(t, value::constant, nullptr, 0));
    fields.back().data.resize(typeSizesMap[t]);
    copyConstantToMemory(fields.back().data.data(), t, arg);
    arg = "";

    return true;
}

bool mesh_compiler::compileUnit::isOtherUnitValue(const type& t, std::string& arg, std::vector<compileField>& fields, counting_type& count_type, /*const*/ std::map<std::string, compileUnit>& unitsMap)
{
    if (unitsMap.find(arg) != unitsMap.end()) {
        if (t != type::mc_none) throw formatInterpreterException(formatInterpreterException::error_code::unsupported_type, "units dont have types");
        if (count_type == counting_type::null) count_type = unitsMap[arg].count_type;
        else if (count_type != unitsMap[arg].count_type) throw formatInterpreterException(formatInterpreterException::error_code::conflicting_unit_fields, "conflicting types: " + countingTypeNamesMap[unitsMap[arg].count_type] + " and " + countingTypeNamesMap[count_type]);
        fields.push_back(compileField(type::mc_unit, value::other_unit, arg.data(), arg.size()));
        arg = "";
        return true;
    }
    return false;
}

mesh_compiler::compileUnit::compileUnit(std::ifstream& file, size_t& line_num, /*const*/ std::map<std::string, compileUnit>* unitsMap) : unitsMap(unitsMap)
{
    std::string line;

    // preamble
    {
        std::getline(file, line);
        ++line_num;
        std::stringstream ss(line);
        std::string word;
        while (ss >> word) {
            std::string arg = word;
            try {
                type t = extractType(word);

                if (isPreambleValue(t, word, this->preamble)) continue;

                if (isOtherUnitValue(t, word, this->preamble, this->count_type, *unitsMap)) continue;

                if (isConstValue(t, word, this->preamble)) continue;

                throw formatInterpreterException(formatInterpreterException::error_code::unknown_statement);
            }
            catch (formatInterpreterException& e) {
                e.fillInfo(line_num, arg);
                throw;
            }
        }
    }

    // buffers
    while (std::getline(file, line)) {
        ++line_num;
        compileBuffer buffer;
        bool fields_def = false;
        std::stringstream ss(line);
        std::string word;
        int word_num = -1;
        while (ss >> word) { // process word
            ++word_num;
            std::string arg = word;

            if (word == "end" && word_num == 0) {
                if (ss >> word) throw formatInterpreterException(formatInterpreterException::error_code::unknown_statement, line_num, word);
                return;
            }

            if (!fields_def) { // preamble

                if (word == ";") {
                    fields_def = true;
                    continue;
                }
                try {
                    type t = extractType(word);

                    if (isPreambleValue(t, word, buffer.preamble)) continue;

                    if (isFieldValue(t, word, buffer.fields, buffer.count_type, this->count_type)) {
                        fields_def = true;
                        continue;
                    }

                    if (isOtherUnitValue(t, word, buffer.preamble, this->count_type, *unitsMap)) continue;

                    if (isConstValue(t, word, buffer.preamble)) continue;

                    throw formatInterpreterException(formatInterpreterException::error_code::unknown_statement);
                }
                catch (formatInterpreterException& e) {
                    e.fillInfo(line_num, arg);
                    throw;
                }
            }

            else { // fields

                try {
                    type t = extractType(word);

                    if (isFieldValue(t, word, buffer.fields, buffer.count_type, this->count_type)) continue;

                    if (isOtherUnitValue(t, word, buffer.fields, buffer.count_type, *unitsMap)) continue;

                    if (isConstValue(t, word, buffer.fields)) continue;

                    throw formatInterpreterException(formatInterpreterException::error_code::unknown_statement);
                }
                catch (formatInterpreterException& e) {
                    e.fillInfo(line_num, arg);
                    throw;
                }
            }
        }
        if (buffer.count_type == counting_type::null) {
            throw formatInterpreterException(formatInterpreterException::error_code::constants_only, line_num, "");
        }
        this->buffers.push_back(buffer);
    }
    throw formatInterpreterException(formatInterpreterException::error_code::no_end, line_num, "");
}


mesh_compiler::fileUnit::fileUnit(std::ifstream& file, const std::string& output_file_, size_t& line_num, /*const*/ std::map<std::string, compileUnit>* unitsMap) : compileUnit(file, line_num, unitsMap), output_file(output_file_)
{
}

mesh_compiler::compilationInfo::compilationInfo(const std::string& format_file, const bool& debug_messages) : debug_messages(debug_messages)
{
    std::ifstream formatFile(format_file, std::ios::in);
    if (!formatFile) {
        throw std::runtime_error("could not open file: " + format_file);
    }

    size_t line_num = 0;
    std::string line;
    while (std::getline(formatFile, line)) {
        ++line_num;
        std::stringstream ss(line);
        std::string word;
        while (ss >> word) {
            std::string arg = word; // for error messages only
            if (word != "begin") throw formatInterpreterException(formatInterpreterException::error_code::unknown_statement, line_num, arg);
            if (!(ss >> word)) throw formatInterpreterException(formatInterpreterException::error_code::no_unit_name, line_num, arg);
            arg += " " + word;
            if (word == "file") {
                if (!(ss >> word)) throw formatInterpreterException(formatInterpreterException::error_code::no_file_name, line_num, arg);
                this->file_units.push_back(fileUnit(formatFile, word, line_num, &units));
                if (this->debug_messages) {
                    std::cout << "file ";
                    this->file_units.back().print();
                }
            }
            else {
                if (units.find(word) != units.end()) throw formatInterpreterException(formatInterpreterException::error_code::unit_redefinition, line_num, arg);
                if (preambleMap.find(word) != preambleMap.end()) throw formatInterpreterException(formatInterpreterException::error_code::name_keyword_collision, line_num, arg);
                if (fieldsMap.find(word) != fieldsMap.end()) throw formatInterpreterException(formatInterpreterException::error_code::name_keyword_collision, line_num, arg);
                this->units.emplace(word, compileUnit(formatFile, line_num, &units));
                if (this->debug_messages) {
                    std::cout << word << " ";
                    this->units[word].print();
                }
            }
        }
    }

    formatFile.close();
    if (debug_messages) std::cout << "format file compilation succeded\n";
}

// ========== COMPILER FUNCTION DEFINITIONS ==========

void mesh_compiler::run(int argc, char** argv)
{
    if (argc == 1) {
        std::string line = "";
        while (1) {
            std::cout << "> ";
            std::getline(std::cin, line);
            if (line == "q") return;
            std::vector<std::string> args;
            std::stringstream ss(line);
            while (ss) {
                std::string a;
                ss >> a;
                if (!a.empty()) args.push_back(a);
            }
            runOnce(args);
        }
    }
    else {
        std::vector<std::string> args;
        for (int i = 1; i < argc; ++i) {
            args.push_back(argv[i]);
        }
        runOnce(args);
    }
}

void mesh_compiler::runOnce(const std::vector<std::string>& args)
{
    if (args.size() == 1 && (args[0] == "-v" || args[0] == "--version")) {
        std::cout << version << std::endl;
        return;
    }
    try {
        compile(args);
    }
    catch (std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }
}

#ifdef _DEBUG
void mesh_compiler::runOnceDebug(const std::vector<std::string>& args)
{
    if (args.size() == 1 && (args[0] == "-v" || args[0] == "--version")) {
        std::cout << version << std::endl;
        return;
    }
    compile(args);
}
#endif // _DEBUG

void mesh_compiler::compile(const std::vector<std::string>& args)
{
    int siz = args.size();
    if (siz == 0) {
        throw std::runtime_error("source file not specified");
    }

    std::string format_file = ".format";
    bool format_specified = false;
    bool debug_messages = false;

    for (int i = 1; i < siz; ++i) {
        if (args[i] == "-f") {
            ++i;
            if (i == siz) throw std::runtime_error("unspecified format file: -f <format file path>");
            if (format_specified) throw std::runtime_error("format file specified more than once");
            format_file = args[i];
            format_specified = true;
        }
        else if (args[i] == "-d") {
            if (debug_messages) throw std::runtime_error("-d flag specified more than once");
            debug_messages = true;
        }
        else if (i == 1) {
            format_file = args[i];
            format_specified = true;
        }
    }
    try {
        try {
            compileFile(args[0], compilationInfo(format_file, debug_messages));
        }
        catch (formatInterpreterException& e) {
            std::cout << e.what() << std::endl;
            throw meshCompilerException("format file compilation ended with errors could not compile file");
        }
    }
    catch (meshCompilerException& e) {
        std::cout << e.what() << std::endl;
    }
}

void mesh_compiler::compileFile(const std::string& filename, compilationInfo ci)
{
    for (fileUnit& fu : ci.file_units) {
        // replace {file} with file name in output file name
        size_t found = fu.output_file.find("{file}");
        if (found != std::string::npos) {
            std::string base_filename = filename.substr(filename.find_last_of("/\\") + 1);
            size_t const p(base_filename.find_last_of('.'));
            fu.output_file.replace(found, 6, base_filename.substr(0, p));
        }

        assimp::readFile(filename, std::bind(mesh_compiler::compileScene, std::placeholders::_1, std::ref(fu)));
    }
}

void mesh_compiler::compileScene(const aiScene* scene, fileUnit fu)
{
    // replace {scene} with scene name in output file name
    size_t found = fu.output_file.find("{scene}");
    if (found != std::string::npos) fu.output_file.replace(found, 7, scene->mName.C_Str());

    // find name and extension
    std::string orig_name = fu.output_file;
    if (fu.count_type == counting_type::per_scene) {
        try {
            std::ofstream fout(fu.output_file, std::ios::out | std::ios::binary);
            if (!fout) {
                throw std::runtime_error("cannot open file: " + fu.output_file);
            }
            fu.put(fout, scene);
            fout.close();
        }
        catch (meshCompilerException& e) {
            std::cout << e.what() << std::endl;
            std::cout << "compilation of scene: " << scene->mName.C_Str() << " ended up with errors.\n";
        }
    }
    else if (fu.count_type == counting_type::per_mesh) {
        int errors = 0;
        for (int i = 0; i < scene->mNumMeshes; ++i) {
            size_t found = fu.output_file.find("{mesh}");
            if (found != std::string::npos) fu.output_file.replace(found, 6, scene->mMeshes[i]->mName.C_Str());
            try {
                std::ofstream fout(fu.output_file, std::ios::out | std::ios::binary);
                if (!fout) {
                    throw std::runtime_error("cannot open file: " + fu.output_file);
                }
                fu.put(fout, scene->mMeshes[i]);
                fout.close();
            }
            catch (meshCompilerException& e) {
                std::cout << e.what() << std::endl;
                std::cout << "compilation of mesh: " << scene->mMeshes[i]->mName.C_Str() << " ended up with errors.\n";
                errors += 1;
            }
            fu.output_file = orig_name; // go back to original name
        }
        if (errors != 0) {
            std::cout << "scene compilation ended with errors\n";
            printf("compiled %d out of %d meshes\n", scene->mNumMeshes - errors, scene->mNumMeshes);
            return;
        }
    }
    else if (fu.count_type == counting_type::per_skeleton) {
        int errors = 0;
        for (int i = 0; i < scene->mNumSkeletons; ++i) {
            size_t found = fu.output_file.find("{skeleton}");
            if (found != std::string::npos) fu.output_file.replace(found, 10, scene->mSkeletons[i]->mName.C_Str());
            try {
                std::ofstream fout(fu.output_file, std::ios::out | std::ios::binary);
                if (!fout) {
                    throw std::runtime_error("cannot open file: " + fu.output_file);
                }
                fu.put(fout, scene->mSkeletons[i]);
                fout.close();
            }
            catch (meshCompilerException& e) {
                std::cout << e.what() << std::endl;
                std::cout << "compilation of skeleton: " << scene->mSkeletons[i]->mName.C_Str() << " ended up with errors.\n";
                errors += 1;
            }
            fu.output_file = orig_name; // go back to original name
        }
        if (errors != 0) {
            std::cout << "scene compilation ended with errors\n";
            printf("compiled %d out of %d skeletons\n", scene->mNumSkeletons - errors, scene->mNumSkeletons);
            return;
        }
    }
    else if (fu.count_type == counting_type::per_animation) {
        int errors = 0;
        for (int i = 0; i < scene->mNumAnimations; ++i) {
            size_t found = fu.output_file.find("{animation}");
            if (found != std::string::npos) fu.output_file.replace(found, 11, scene->mAnimations[i]->mName.C_Str());
            try {
                std::ofstream fout(fu.output_file, std::ios::out | std::ios::binary);
                if (!fout) {
                    throw std::runtime_error("cannot open file: " + fu.output_file);
                }
                fu.put(fout, scene->mAnimations[i]);
                fout.close();
            }
            catch (meshCompilerException& e) {
                std::cout << e.what() << std::endl;
                std::cout << "compilation of animation: " << scene->mAnimations[i]->mName.C_Str() << " ended up with errors.\n";
                errors += 1;
            }
            fu.output_file = orig_name; // go back to original name
        }
        if (errors != 0) {
            std::cout << "scene compilation ended with errors\n";
            printf("compiled %d out of %d animations\n", scene->mNumAnimations - errors, scene->mNumAnimations);
            return;
        }
    }
    else if (fu.count_type == counting_type::per_animation_channel) {
        for (int i = 0; i < scene->mNumAnimations; ++i) {
            int errors = 0;
            size_t found = fu.output_file.find("{animation}");
            if (found != std::string::npos) fu.output_file.replace(found, 11, scene->mAnimations[i]->mName.C_Str());
            std::string orig_name2 = fu.output_file;
            for (int j = 0; j < scene->mAnimations[i]->mNumChannels; ++i) {
                size_t found = fu.output_file.find("{channel}");
                if (found != std::string::npos) fu.output_file.replace(found, 9, scene->mAnimations[i]->mChannels[j]->mNodeName.C_Str());
                try {
                    std::ofstream fout(fu.output_file, std::ios::out | std::ios::binary);
                    if (!fout) {
                        throw std::runtime_error("cannot open file: " + fu.output_file);
                    }
                    fu.put(fout, scene->mAnimations[i]->mChannels[j]);
                    fout.close();
                }
                catch (meshCompilerException& e) {
                    std::cout << e.what() << std::endl;
                    std::cout << "compilation of animation channel: " << scene->mAnimations[i]->mChannels[j]->mNodeName.C_Str() << " ended up with errors.\n";
                    errors += 1;
                }
                fu.output_file = orig_name2;
            }
            fu.output_file = orig_name; // go back to original name
            if (errors != 0) {
                std::cout << "animation compilation ended with errors\n";
                printf("compiled %d out of %d animation channels\n", scene->mAnimations[i]->mNumChannels - errors, scene->mAnimations[i]->mNumChannels);
                return;
            }
        }
    }
}
