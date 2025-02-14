#include "pfield.h"

std::vector<mc::anyType::value> mc::bpfield::evaluate(const Inode::ptr node) const
{
	return evaluate1(node->getChildNodeOfType(buff->c, 0), buff);
}

std::vector<mc::anyType::value> mc::bpfield::evaluate1(const Inode::ptr node, const buffer* buffe) const
{
	std::vector<anyType::value> out;

	switch (p)
	{
	case ptype::buffer_size:
		out.push_back(anyType::getValue(s, buffe->getSize(node)));
		break;
	case ptype::entry_size:
		out.push_back(anyType::getValue(s, buffe->getEntrySize()));
		break;
	case ptype::entries_per_buffer:
		out.push_back(anyType::getValue(s, node->getCount()));
		break;
	case ptype::field_size:
		for (const field::ptr& f : buffe->fields) {
			size_t count = f->getCount();
			for (size_t i = 0; i < count; ++i) {
				out.push_back(anyType::getValue(s, f->getSize() / count));
			}
		}
		break;
	case ptype::fields_per_entry:
		out.push_back(anyType::getValue(s, buffe->fieldsPerEntry()));
		break;
	case ptype::fields_per_buffer:
		out.push_back(anyType::getValue(s, buffe->fieldsPerEntry() * node->getCount()));
		break;
	default:
		throw std::logic_error("invalid ptype");
		break;
	}

	return out;
}

mc::ctype::code mc::bpfield::getCountingType() const
{
	return ctype::null;
}

std::vector<mc::anyType::value> mc::pfield::evaluate(const Inode::ptr node) const
{
	std::vector<anyType::value> out;

	switch (p)
	{
	case ptype::buffers_per_unit:
		out.push_back(anyType::getValue(s, u->buffers.size()));
		break;
	case ptype::entries_per_unit:
		out.push_back(anyType::getValue(s, u->getEntriesCount(node)));
		break;
	case ptype::fields_per_unit:
		out.push_back(anyType::getValue(s, u->getFieldsCount(node)));
		break;
	case ptype::buffer_size:
	case ptype::entry_size:
	case ptype::entries_per_buffer:
	case ptype::field_size:
	case ptype::fields_per_entry:
	case ptype::fields_per_buffer:
		for (const buffer& b : u->buffers) {
			std::vector<anyType::value> res = this->bpfield::evaluate1(node->getChildNodeOfType(b.c, 0), &b);
			out.insert(out.end(), res.begin(), res.end());
		}
		break;
	default:
		throw std::logic_error("invalid ptype");
		break;
	}

	return out;
}

mc::ctype::code mc::pfield::getCountingType() const
{
	return ctype::null;
}

oop_ptr_define(mc::field, mc::bpfield)
oop_ptr_define(mc::field, mc::pfield)
