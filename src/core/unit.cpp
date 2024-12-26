#include "unit.h"
#include <deque>
#include "Inode.h"
#include <cassert>
#include "fields/pfield.h"
#include <iostream>


void mc::unit::output(std::ofstream& file, const Inode::ptr node, printMode pm)
{
    assert(c == node->c);

    for (field::ptr& f : preamble) {
        if (f.gettable<Ipfield>()) {
            f.get<Ipfield>()->u = this;
        }
        f->output(file, node, pm);
    }

    for (buffer buff : buffers) {
        if (pm == printMode::plainText) file << "\n";
        buff.output(file, node, pm);
    }
}

void mc::fileUnit::compile(const Inode::ptr node)
{
    std::cout << "compiling node of name: " << node->getName() << "\n";

    std::string orig_name = output_file;
    changeName(ctype::patterns.at(node->c), node->getName());

    if (c == node->c) {
        std::ofstream fout(output_file, std::ios::out | std::ios::binary);
        if (!fout) {
            throw std::runtime_error("cannot open file: " + output_file);
        }
        output(fout, node, mode);
        fout.close();
    }

    else {
        ctype::code nodeChild = c;
        while (ctype::parents.at(nodeChild) != node->c) {
            nodeChild = ctype::parents.at(nodeChild);
            if (nodeChild == ctype::per_scene)
                throw std::logic_error("nodes counting type is not an ancestor of this units counting type");
        }


        for (size_t i = 0; i < node->getChildNodeCount(nodeChild); ++i) {
            compile(node->getChildNodeOfType(nodeChild, i));
        }
    }

    output_file = orig_name;
}

void mc::fileUnit::changeName(const std::string& pattern, const std::string& newName)
{
    size_t found = output_file.find(pattern);
    if (found != std::string::npos) output_file.replace(found, pattern.length(), newName);
}

void mc::fileUnit::withChangedName(
    const std::string& pattern, const std::string& newName,
    const std::function<void(fileUnit*)>& func
) {
    std::string orig_name = output_file;
    changeName(pattern, newName);

    func(this);

    output_file = orig_name;
}
