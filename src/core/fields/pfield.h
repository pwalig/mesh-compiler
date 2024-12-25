#pragma once
#include "sfield.h"
#include "../types/ptype.h"
#include "../buffer.h"
#include "../unit.h"

namespace mc {

	class Ibpfield : virtual public field {
	public:
		const ptype::code p;
		buffer* buff;
		oop_ptr_base_declare(field) = 0;
	};

	class Ipfield : public Ibpfield {
	public:
		unit* u;
		oop_ptr_base_declare(field) = 0;
	};

	template<typename T>
	class bpfield : public sfield<T>, public Ibpfield {
	public:

		std::vector<T> evaluate(const Inode::ptr node) const override;

		oop_ptr_template_child_define(field, bpfield)
	};

	template<typename T>
	inline std::vector<T> bpfield<T>::evaluate(const Inode::ptr node) const
	{
		std::vector<T> out;

		switch (p)
		{
		case ptype::buffer_size:
			out.push_back(buff->getSize<T>(node));
			break;
		case ptype::entry_size:
			out.push_back(buff->getEntrySize<T>());
			break;
		case ptype::entries_per_buffer:
			out.push_back(node->getCount());
			break;
		case ptype::field_size:
			for (const field::ptr& f : buff->fields) {
				out.push_back(f->getSize<T>());
			}
			break;
		case ptype::fields_per_entry:
			out.push_back(buff->fields.size());
			break;
		case ptype::fields_per_buffer:
			out.push_back(buff->fields.size() * node->getCount());
			break;
		default:
			throw std::logic_error("invalid ptype");
			break;
		}

		return out;
	}


	template<typename T>
	class pfield : public bpfield<T>, public Ipfield {
	public:

		std::vector<T> evaluate(const Inode::ptr node) const override;

		oop_ptr_template_child_define(field, pfield)
	};

	template<typename T>
	inline std::vector<T> pfield<T>::evaluate(const Inode::ptr node) const
	{
		std::vector<T> out;

		switch (p)
		{
		case ptype::buffers_per_unit:
			out.push_back(u->buffers.size());
			break;
		case ptype::entries_per_unit:
			out.push_back(u->getEntriesCount<T>(node));
			break;
		case ptype::fields_per_unit:
			out.push_back(u->getFieldsCount<T>());
			break;
		case ptype::buffer_size:
		case ptype::entry_size:
		case ptype::entries_per_buffer:
		case ptype::field_size:
		case ptype::fields_per_entry:
		case ptype::fields_per_buffer:
			for (const buffer& b : u->buffers) {
				buff = &b;
				std::vector<T> res = this->bpfield<T>::evaluate(node);
				out.insert(out.end(), res.begin(), res.end());
			}
			break;
		default:
			throw std::logic_error("invalid ptype");
			break;
		}

		return out;
	}
}