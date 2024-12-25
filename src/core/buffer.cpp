#include "buffer.h"
#include "fields/pfield.h"

void mc::buffer::output(std::ofstream& file, const Inode* node)
{
    size_t count = node->getChildNodeCount(c);

    for (field::ptr& f : preamble) {
        if (f.gettable<Ibpfield>()) {
            f.get<Ibpfield>()->buff = this;
        }
        f->output(file, node);
    }

    for (size_t i = 0; i < count; ++i) {
        for (field::ptr& f : fields) {
            f->output(file, node->getChildNodeOfType(c, i));
        }
    }
}
