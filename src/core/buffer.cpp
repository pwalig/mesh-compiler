#include "buffer.h"
#include "fields/pfield.h"

mc::buffer::buffer(const rapidjson::Value& json) : c(ctype::null)
{
    assert(json.IsObject());

    assert(json.HasMember("preamble"));
    const rapidjson::Value& p = json["preamble"];
    assert(p.IsArray());
    for (rapidjson::SizeType i = 0; i < p.Size(); i++) {
        preamble.push_back(field::getPtr(p[i]));
    }

    assert(json.HasMember("fields"));
    const rapidjson::Value& f = json["fields"];
    assert(f.IsArray());
    for (rapidjson::SizeType i = 0; i < f.Size(); i++) {
        fields.push_back(field::getPtr(f[i]));
    }
}

void mc::buffer::output(std::ofstream& file, const Inode::ptr node, mc::printMode pm)
{
    size_t count = node->getChildNodeCount(c);

    for (field::ptr& f : preamble) {
        if (f.gettable<Ibpfield>()) {
            f.get<Ibpfield>()->buff = this;
        }
        f->output(file, node->getChildNodeOfType(c, 0), pm);
    }

    for (size_t i = 0; i < count; ++i) {
        for (field::ptr& f : fields) {
            f->output(file, node->getChildNodeOfType(c, i), pm);
        }
    }
}
