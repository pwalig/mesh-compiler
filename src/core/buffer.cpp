#include "buffer.h"
#include "fields/pfieldT.h"
#include "fields/pfield.h"
#include "exceptions/jsonException.h"
#include "fields/ufield.h"
#include "compilation-info.h"

mc::buffer::buffer(const rapidjson::Value& json) : c(ctype::null)
{
    assert(json.IsObject());

    assert(json.HasMember("fields"));
    const rapidjson::Value& f = json["fields"];
    assert(f.IsArray());
    for (rapidjson::SizeType i = 0; i < f.Size(); i++) {
        fields.push_back(field::getPtr(f[i], field::location::buffer_field));

        updateCtype(c, fields.back()->getCountingType(), [this](ctype::code new_, ctype::code old) {
            if (new_ != old) throw jsonException(
                "conflicting counting types in buffer fields\n" +
                ctype::names.at(fields.back()->getCountingType()) +
                " conficts with " + ctype::names.at(c));
            }
        );
    }

    if (c == ctype::null) throw std::runtime_error("buffer of unknown counting type");

    assert(json.HasMember("preamble"));
    const rapidjson::Value& p = json["preamble"];
    assert(p.IsArray());
    for (rapidjson::SizeType i = 0; i < p.Size(); i++) {
        const rapidjson::Value& jnode = p[i];
        preamble.push_back(field::getPtr(jnode, field::location::buffer_preamble));

        updateCtype(c, preamble.back()->getCountingType(), [this, &jnode](ctype::code new_, ctype::code old) {
            if (new_ != ctype::parents.at(old)) throw jsonException(
                "preamble field's counting type conficts with buffer counting type\npreamble field " +
                std::string(jnode["value"].IsString() ? jnode["value"].GetString() : "") + "'s counting type: " +
                ctype::names.at(preamble.back()->getCountingType()) +
                ", buffer's counting type: " + ctype::names.at(c));
            }
        );
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

size_t mc::buffer::getEntrySize() const
{
    size_t siz = 0;
    for (const field::ptr& f : fields)
        siz += f->getSize();
    return siz;
}

size_t mc::buffer::getSize(const Inode::ptr node) const
{
    return getEntrySize() * node->getCount();
}

void mc::updateCtype(ctype::code& current, ctype::code new_,
    std::function<void(ctype::code new_, ctype::code old)> conflictCheck)
{
    if (new_ != ctype::null) {
        if (current == ctype::null) current = new_;
        else conflictCheck(new_, current);
    }
}
