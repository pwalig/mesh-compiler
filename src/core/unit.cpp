#include "unit.h"
#include <deque>
#include "Inode.h"
#include <cassert>


void mc::unit::output(std::ofstream& file, const Inode* node)
{
    assert(c == node->c);

    for (field::ptr& f : preamble) {
        f->output(file, node, this);
    }

    for (buffer buff : buffers) {
        buff.output(file, node, this);
    }
}

void mc::fileUnit::compile(const Inode* node)
{
    std::string orig_name = output_file;
    changeName(ctype::patterns.at(node->c), node->getName());

    if (c == node->c) {
        std::ofstream fout(output_file, std::ios::out | std::ios::binary);
        if (!fout) {
            throw std::runtime_error("cannot open file: " + output_file);
        }
        output(fout, node);
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
