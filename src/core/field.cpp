#include "field.h"
#include <stdexcept>

mc::field::field(
    const stype::code& st, const vtype::code& vt, const void* data_source
) : field(st, vt, data_source, stype::sizes.at(st))
{
}

mc::field::field(
    const stype::code& st, const vtype::code& vt,
    const void* data_source, const size_t& data_amount
) : s(st), v(vt)
{
    if (s == stype::null)
        throw std::logic_error("unable to construct field of stype: null");
    if (v == vtype::null)
        throw std::logic_error("unable to construct field of vtype: null");
    if (data_source != nullptr) {
        data.resize(data_amount);
        memcpy(data.data(), data_source, data_amount);
    }
}

void mc::field::output(std::ofstream& file, const Inode* node)
{
}
