#include "buffer.h"
#include "fields/pfieldT.h"
#include "fields/pfield.h"
#include "exceptions/jsonException.h"

mc::buffer::buffer(const rapidjson::Value& json) : c(ctype::null)
{
    assert(json.IsObject());

    assert(json.HasMember("fields"));
    const rapidjson::Value& f = json["fields"];
    assert(f.IsArray());
    for (rapidjson::SizeType i = 0; i < f.Size(); i++) {
        fields.push_back(field::getPtr(f[i], field::location::buffer_field));
        ctype::code ct = fields.back()->getCountingType();
        if (ct != ctype::null) {
            if (c == ctype::null) c = ct;
            else if (c != ct) throw jsonException("conflicting counting types in buffer fields\n"
                + ctype::names.at(ct) + " conficts with " + ctype::names.at(c));
        }
    }

    if (c == ctype::null) throw std::runtime_error("buffer of unknown counting type");

    assert(json.HasMember("preamble"));
    const rapidjson::Value& p = json["preamble"];
    assert(p.IsArray());
    for (rapidjson::SizeType i = 0; i < p.Size(); i++) {
        preamble.push_back(field::getPtr(p[i], field::location::buffer_preamble));
        ctype::code ct = preamble.back()->getCountingType();
        if (ct != ctype::null) {
            if (c == ctype::null) c = ct;
            else if (ctype::parents.at(c) != ct) throw jsonException("preamble field's counting type conficts with buffer counting type\npreamble field "
                + std::string(p[i]["value"].GetString()) + "'s counting type: "
                + ctype::names.at(ct) + ", buffer's counting type: " + ctype::names.at(c));
        }
    }
}

void mc::buffer::output(std::ofstream& file, const Inode::ptr node, mc::printMode pm)
{
    size_t count = node->getChildNodeCount(c);

    for (field::ptr& f : preamble) {
        if (f.gettable<Ibpfield>()) f.get<Ibpfield>()->buff = this;
        if (f.gettable<bpfield>()) f.get<bpfield>()->buff = this;
        f->output(file, node, pm);
    }

    for (size_t i = 0; i < count; ++i) {
        for (field::ptr& f : fields) {
            f->output(file, node->getChildNodeOfType(c, i), pm);
        }
    }
}
