#include "buffer.h"

void mc::buffer::output(std::ofstream& file, const Inode* node)
{
    size_t count = node->getChildNodeCount(c);

    for (field::ptr& f : preamble) {
        f->output(file, node);
    }

    for (size_t i = 0; i < count; ++i) {
        for (field::ptr& f : fields) {
            f->output(file, node->getChildNodeOfType(c, i));
        }
    }
}
