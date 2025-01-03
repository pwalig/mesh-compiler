#include "unit.h"
#include <deque>
#include "Inode.h"
#include <cassert>
#include "fields/pfieldT.h"
#include <iostream>
#include "fields/pfield.h"
#include "../jsonTools.h"
#include "exceptions/jsonException.h"

mc::unit::unit(const rapidjson::Value& json) : c(ctype::null)
{
    assert(json.IsObject());

    if (json.HasMember("preamble")) {
        const rapidjson::Value& p = json["preamble"];
        if (!p.IsArray()) throw jsonException("unit preamble was not a json array");

        for (rapidjson::SizeType i = 0; i < p.Size(); i++) {
            preamble.push_back(field::getPtr(p[i], field::location::main_preamble));
            ctype::code ct = preamble.back()->getCountingType();
            if (ct != ctype::null) {
                if (c == ctype::null) c = ct;
                else if (c != ct) throw jsonException("conflicting counting types in unit preamble\n"
                    + ctype::names.at(ct) + " conficts with " + ctype::names.at(c));
            }
        }
    }

    if (json.HasMember("buffers")) {
        const rapidjson::Value& b = json["buffers"];
        if (!b.IsArray()) throw jsonException("unit buffers was not a json array");

        for (rapidjson::SizeType i = 0; i < b.Size(); i++) {
            buffers.push_back(buffer(b[i]));
            ctype::code ct = ctype::parents.at(buffers.back().c);
            if (ct != ctype::null) {
                if (c == ctype::null) c = ct;
                else if (c != ct) throw jsonException("buffer counting type conflicts with unit's counting type\nbuffer's counting type: "
                    + ctype::names.at(buffers.back().c) + ", unit's counting type: " + ctype::names.at(c));
            }
        }
    }

    if (c == ctype::null) throw jsonException("unit of unknown counting type");
}

void mc::unit::output(std::ofstream& file, const Inode::ptr node, printMode pm)
{
    assert(c == node->c);

    for (field::ptr& f : preamble) {
        if (f.gettable<Ipfield>()) f.get<Ipfield>()->u = this;
        if (f.gettable<pfield>()) f.get<pfield>()->u = this;
        f->output(file, node, pm);
    }

    for (buffer buff : buffers) {
        if (pm == printMode::plainText) file << "\n";
        buff.output(file, node, pm);
    }
}

mc::fileUnit::fileUnit(const rapidjson::Value& json) : unit(json), output_file(json["output_file"].GetString())
{
    std::string ftype = jsonTools::getAnyMember(json, {"print_mode", "printMode", "print-mode"});
    if (ftype != "") {
        assert(json[ftype.c_str()].IsString());
        std::string omode = json[ftype.c_str()].GetString();
        std::vector<std::string> available = { "plain_text", "plainText", "plain-text" };
        if (std::find(available.begin(), available.end(), omode) != available.end()) mode = mc::printMode::plainText;
        else if (omode == "binary") mode = mc::printMode::binary;
        else throw jsonException("invalid printMode");
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
