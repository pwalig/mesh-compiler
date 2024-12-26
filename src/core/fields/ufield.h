#pragma once
#include "../field.h"

namespace mc {
	class ufield : public field {
	public:
		const std::string unitName;
		const std::unordered_map<std::string, unit>* unitsMap;

		void output(std::ofstream& file, const Inode::ptr node, printMode pm) const override;
	};
}