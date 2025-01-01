#pragma once
#include "sfieldT.h"
#include "../types/ptype.h"
#include "../buffer.h"
#include "../unit.h"

namespace mc {

	class Ibpfield : virtual public field {
	public:
		const buffer* buff;
		const ptype::code p;

		Ibpfield(ptype::code pt) : p(pt), buff(nullptr) {}
		oop_ptr_base_declare(field) = 0;
	};

	class Ipfield : virtual public field {
	public:
		const ptype::code p;
		const unit* u;
		const buffer* buff;
		Ipfield(ptype::code pt) : p(pt), u(nullptr), buff(nullptr) {}
		oop_ptr_base_declare(field) = 0;
	};

	template<typename T>
	class bpfieldT : public sfieldT<T>, public Ibpfield {
	public:
		bpfieldT(ptype::code pt) : Ibpfield(pt) {}

		std::vector<T> evaluate(const Inode::ptr node) const override;
		std::vector<T> evaluate1(const Inode::ptr node, const buffer* buffe) const;
		ctype::code getCountingType() const override;

		oop_ptr_template_child_define(field, bpfieldT)
	};

	template<typename T>
	inline std::vector<T> bpfieldT<T>::evaluate(const Inode::ptr node) const
	{
		return evaluate1(node->getChildNodeOfType(buff->c, 0), buff);
	}

	template<typename T>
	inline std::vector<T> bpfieldT<T>::evaluate1(const Inode::ptr node, const buffer* buffe) const
	{
		std::vector<T> out;

		switch (p)
		{
		case ptype::buffer_size:
			out.push_back(buffe->getSize<T>(node));
			break;
		case ptype::entry_size:
			out.push_back(buffe->getEntrySize<T>());
			break;
		case ptype::entries_per_buffer:
			out.push_back((T)node->getCount());
			break;
		case ptype::field_size:
			for (const field::ptr& f : buffe->fields) {
				out.push_back((T)f->getSize());
			}
			break;
		case ptype::fields_per_entry:
			out.push_back((T)buffe->fields.size());
			break;
		case ptype::fields_per_buffer:
			out.push_back((T)(buffe->fields.size() * node->getCount()));
			break;
		default:
			throw std::logic_error("invalid ptype");
			break;
		}

		return out;
	}

	template<typename T>
	inline ctype::code bpfieldT<T>::getCountingType() const
	{
		return ctype::null;
	}


	template<typename T>
	class pfieldT : public bpfieldT<T>, public Ipfield {
	public:
		pfieldT(ptype::code pt) : bpfieldT<T>(pt), Ipfield(pt) {}

		std::vector<T> evaluate(const Inode::ptr node) const override;
		ctype::code getCountingType() const override;

		oop_ptr_template_child_define(field, pfieldT)
	};

	template<typename T>
	inline std::vector<T> pfieldT<T>::evaluate(const Inode::ptr node) const
	{
		std::vector<T> out;

		switch (p)
		{
		case ptype::buffers_per_unit:
			out.push_back((T)u->buffers.size());
			break;
		case ptype::entries_per_unit:
			out.push_back(u->getEntriesCount<T>(node));
			break;
		case ptype::fields_per_unit:
			out.push_back(u->getFieldsCount<T>(node));
			break;
		case ptype::buffer_size:
		case ptype::entry_size:
		case ptype::entries_per_buffer:
		case ptype::field_size:
		case ptype::fields_per_entry:
		case ptype::fields_per_buffer:
			for (const buffer& b : u->buffers) {
				std::vector<T> res = this->bpfieldT<T>::evaluate1(node->getChildNodeOfType(b.c, 0), &b);
				out.insert(out.end(), res.begin(), res.end());
			}
			break;
		default:
			throw std::logic_error("invalid ptype");
			break;
		}

		return out;
	}
	template<typename T>
	inline ctype::code pfieldT<T>::getCountingType() const
	{
		return ctype::null;
	}
}
