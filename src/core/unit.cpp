#include "unit.h"
#include <deque>
#include "Inode.h"
#include <cassert>
#include "fields/pfieldT.h"
#include <iostream>
#include "fields/pfield.h"
#include "../jsonTools.h"
#include "exceptions/jsonException.h"
#include "exceptions/compileException.h"
#include "compilation-info.h"
#include "fields/ufield.h"
#include "fields/vfield.h"
#include <sstream>
#include "exceptions/formatException.h"

mc::unit::unit(std::ifstream& file, compilationContext& context) : c(ctype::null)
{
    std::string line;

    // preamble
    std::getline(file, line); // line of the preamble
    context.linenum++;
    std::stringstream ss(line);
    std::string word;
    while (ss >> word) {
        preamble.push_back(field::getPtr(word, mc::field::location::main_preamble, context));

        updateCtype(c, preamble.back()->getCountingType(), [this, context](ctype::code new_, ctype::code old) {
            if (old != new_) throw formatException("conflicting counting types in unit preamble\n"
                + ctype::names.at(new_) + " conficts with " + ctype::names.at(c), context);
            });
    }

    bool endEncountered = false;

    while (file >> word) {
        if (word == "end") {
            endEncountered = true;
            break;
        }
        buffer b;
        std::getline(file, line);
		context.linenum++;
        ss = std::stringstream(line);
        bool inPreamble = true;
        ctype::code preambleCT = ctype::null;
        do {
            if (word == ";") inPreamble = false;
            else {
                if (vfield::gettable(word)) inPreamble = false;
                if (inPreamble) {
                    b.preamble.push_back(field::getPtr(word, mc::field::location::buffer_preamble, context));
                    updateCtype(preambleCT, b.preamble.back()->getCountingType(), [context](ctype::code new_, ctype::code old) {
                        if (new_ != old) throw formatException("conflicting counting types in buffer preamble\n" +
                            ctype::names.at(new_) + " conflicts with " + ctype::names.at(old), context);
                        });
                }
                else {
                    b.fields.push_back(field::getPtr(word, mc::field::location::buffer_field, context));
                    updateCtype(b.c, b.fields.back()->getCountingType(), [preambleCT, context](ctype::code new_, ctype::code old) {
                        if (new_ != old) throw formatException("conflicting counting types in buffer fields\n" +
                            ctype::names.at(new_) + " conflicts with " + ctype::names.at(old), context);
                        else if (preambleCT != ctype::null && ctype::parents.at(new_) != preambleCT) throw formatException(
                            "conflicting counting types between buffer field and buffer preamble\nfield's counting type: " +
                            ctype::names.at(new_) + " conflicts with preamble's counting type: " + ctype::names.at(preambleCT), context);
                        });
                }
            }
        } while (ss >> word);
        if (b.c == ctype::null) throw formatException("buffer of unknown counting type", context);
        buffers.push_back(b);

        updateCtype(c, ctype::parents.at(buffers.back().c), [this, context](ctype::code new_, ctype::code old) {
            if (old != new_) throw formatException("buffer counting type conflicts with unit's counting type\nbuffer's counting type: "
                + ctype::names.at(buffers.back().c) + ", unit's counting type: " + ctype::names.at(c), context);
            });
    }

    if (!endEncountered) throw formatException("missing end keyword", context);
}

mc::unit::unit(const rapidjson::Value& json) : c(ctype::null)
{
    assert(json.IsObject());

    if (json.HasMember("preamble")) {
        const rapidjson::Value& p = json["preamble"];
        if (!p.IsArray()) throw jsonException("unit preamble was not a json array");

        for (rapidjson::SizeType i = 0; i < p.Size(); i++) {
            preamble.push_back(field::getPtr(p[i], field::location::main_preamble));

            updateCtype(c, preamble.back()->getCountingType(), [this](ctype::code new_, ctype::code old) {
                if (old != new_) throw jsonException("conflicting counting types in unit preamble\n"
                    + ctype::names.at(new_) + " conficts with " + ctype::names.at(c));
                });
        }
    }

    if (json.HasMember("buffers")) {
        const rapidjson::Value& b = json["buffers"];
        if (!b.IsArray()) throw jsonException("unit buffers was not a json array");

        for (rapidjson::SizeType i = 0; i < b.Size(); i++) {
            buffers.push_back(buffer(b[i]));

            updateCtype(c, ctype::parents.at(buffers.back().c), [this](ctype::code new_, ctype::code old) {
                if (old != new_) throw jsonException("buffer counting type conflicts with unit's counting type\nbuffer's counting type: "
                    + ctype::names.at(buffers.back().c) + ", unit's counting type: " + ctype::names.at(c));
                });
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

size_t mc::unit::getSize(const Inode::ptr node) const
{
    size_t siz = 0;
    for (const buffer& buff : buffers)
        siz += buff.getSize(node->getChildNodeOfType(buff.c, 0));
    return siz;
}

size_t mc::unit::getEntriesCount(const Inode::ptr node) const
{
    size_t siz = 0;
    for (const buffer& buff : buffers)
        siz += node->getChildNodeCount(buff.c);
    return siz;
}

size_t mc::unit::getFieldsCount(const Inode::ptr node) const
{
    size_t siz = 0;
    for (const buffer& buff : buffers)
        siz += buff.fieldsPerEntry() * node->getChildNodeCount(buff.c);
    return siz;
}

mc::fileUnit::fileUnit(std::ifstream& file, const std::string& output_file_, compilationContext& context) :
    output_file(output_file_), unit(file, context) { }

mc::fileUnit::fileUnit(const rapidjson::Value& json) : unit(json), output_file(json["output_file"].GetString())
{
    std::string ftype = jsonTools::getAnyMember(json, {"print_mode", "printMode", "print-mode"});
    if (ftype != "") {
        assert(json[ftype.c_str()].IsString());
        std::string omode = json[ftype.c_str()].GetString();
        if (std::find(plainTextSpellings.begin(), plainTextSpellings.end(), omode) != plainTextSpellings.end()) mode = mc::printMode::plainText;
        else if (omode == binarySpelling) mode = mc::printMode::binary;
        else throw jsonException("invalid printMode");
    }
}

void mc::fileUnit::compile(const Inode::ptr node, bool debug)
{
    // if (debug) std::cout << "\tcompiling node of name: " << node->getName() << "\n";

    std::string orig_name = output_file;
    changeName(ctype::patterns.at(node->c), node->getName());

    if (c == node->c) {
        if (debug) std::cout << "\t" << output_file << "\n";
        std::ofstream fout(output_file, std::ios::out | (mode == printMode::binary ? std::ios::binary : 0));
        if (!fout) {
            throw compileException("cannot open file: " + output_file);
        }
        output(fout, node, mode);
        fout.close();
    }

    else {
        ctype::code nodeChild = c;
        while (ctype::parents.at(nodeChild) != node->c) {
            nodeChild = ctype::parents.at(nodeChild);
            if (nodeChild == ctype::per_scene)
                throw compileException("node's counting type is not an ancestor of this unit's counting type");
        }


        for (size_t i = 0; i < node->getChildNodeCount(nodeChild); ++i) {
            compile(node->getChildNodeOfType(nodeChild, i), debug);
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
