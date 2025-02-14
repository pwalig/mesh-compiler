#ifdef _DEBUG

#include "tests.h"

#include <iostream>
#include <chrono>
#include <fstream>
#include <functional>

#include <assimp/postprocess.h>

#include "meshReader.h"
#include "assimp-integration/reader.h"
#include "assimp-integration/inspector.h"
#include "core/compilation-info.h"
#include "core/fields/vfieldT.h"
#include "core/fields/pfieldT.h"
#include "core/fields/cfieldT.h"
#include "core/fields/cfield.h"
#include "core/fields/vfield.h"
#include "core/fields/pfield.h"
#include "core/exceptions/jsonException.h"
#include "core/exceptions/compileException.h"

void printMesh(const mesh& m) {
    std::cout << "Mesh info\nindices: ";
    for (const unsigned int& indi : m.indices) std::cout << indi << " ";
    std::cout << "\n verts: ";
    for (const float& vert : m.verts) std::cout << vert << " ";
    std::cout << "\n normals: ";
    for (const float& vert : m.normals) std::cout << vert << " ";
    std::cout << "\n uvs: ";
    for (const float& vert : m.uvs) std::cout << vert << " ";
    std::cout << "\n tangents: ";
    for (const float& vert : m.tangents) std::cout << vert << " ";
    std::cout << "\n bitangents: ";
    for (const float& vert : m.bitangents) std::cout << vert << " ";
    std::cout << "\n indexes: ";
    for (const int vert : m.bone_indexes) std::cout << vert << " ";
    std::cout << "\n weights: ";
    for (const float vert : m.bone_weights) std::cout << vert << " ";
    std::cout << std::endl;
}

void readMeshFile(const std::string& filepath, mesh& me) {
    std::ifstream fin(filepath, std::ios::out | std::ios::binary);
    if (!fin) {
        std::cout << "Cannot open file!" << std::endl;
        exit(1);
    }

    unsigned short max_bones;
    fin.read((char*) & max_bones, sizeof(unsigned short));

    mesh_reader::readBuffer<unsigned int, unsigned int>(fin, me.indices);
    mesh_reader::readBuffer<float, unsigned int>(fin, me.verts);
    mesh_reader::readBuffer<float, unsigned int>(fin, me.normals);
    mesh_reader::readBuffer<float, unsigned int>(fin, me.uvs);
    mesh_reader::readBuffer<float, unsigned int>(fin, me.tangents);
    mesh_reader::readBuffer<float, unsigned int>(fin, me.bitangents);
    mesh_reader::readBuffer<int, unsigned int>(fin, me.bone_indexes);
    mesh_reader::readBuffer<float, unsigned int>(fin, me.bone_weights);

    fin.close();
}

void readMesh(const aiScene* scene, mesh& me) {
    if (scene->mNumMeshes > 0) {
        aiMesh* m = scene->mMeshes[0];
        std::cout << m->mName.C_Str() << std::endl;
        for (unsigned int i = 0; i < m->mNumVertices; ++i) {
            me.verts.push_back(m->mVertices[i].x);
            me.verts.push_back(m->mVertices[i].y);
            me.verts.push_back(m->mVertices[i].z);
            me.normals.push_back(m->mNormals[i].x);
            me.normals.push_back(m->mNormals[i].y);
            me.normals.push_back(m->mNormals[i].z);
            me.uvs.push_back(m->mTextureCoords[0][i].x);
            me.uvs.push_back(m->mTextureCoords[0][i].y);
            me.uvs.push_back(m->mTextureCoords[0][i].z);
            me.tangents.push_back(m->mTangents[i].x);
            me.tangents.push_back(m->mTangents[i].y);
            me.tangents.push_back(m->mTangents[i].z);
            me.bitangents.push_back(m->mBitangents[i].x);
            me.bitangents.push_back(m->mBitangents[i].y);
            me.bitangents.push_back(m->mBitangents[i].z);
        }
        for (unsigned int i = 0; i < m->mNumFaces; ++i) {
            for (unsigned int j = 0; j < m->mFaces[i].mNumIndices; ++j) {
                me.indices.push_back(m->mFaces[i].mIndices[j]);
            }
        }
        if (m->HasBones()) {
            assimp::meshWeights<int, float, MAX_BONE_INFLUENCE> mw(m);
            for (unsigned int i = 0; i < m->mNumVertices; ++i) {
                for (int j = 0; j < MAX_BONE_INFLUENCE; ++j) {
                    me.bone_indexes.push_back(mw.vertices[i].bone_ids[j]);
                    me.bone_weights.push_back(mw.vertices[i].weights[j]);
                }
            }
            assimp::skeleton skeleton(m);
            std::cout << "skeleton: ";
            for (const assimp::skeleton::bone& b : skeleton.bones) {
                std::cout << b.position.x << " " << b.position.y << " " << b.position.z << " | ";
            }
            std::cout << "\n";
        }
    }
}

void mainTest() {
    std::function<void(const aiScene* scene)> nothing = [](const aiScene* scene) {};
    mesh me;
    const auto start{ std::chrono::steady_clock::now() };
    assimp::readFile("../test/bones2.fbx", std::bind(readMesh, std::placeholders::_1, std::ref(me)));
    assimp::readFile("../test/anim-test.glb", std::bind(assimp::printScene, std::placeholders::_1, true));
    //ReadFile("Ghost2.glb", nothing);
    const auto end{ std::chrono::steady_clock::now() };
    const std::chrono::duration<double> elapsed_seconds{ end - start };

    std::cout << "Vert Count: " << me.verts.size() / 3 << '\n';
    std::cout << "Indices Count: " << me.indices.size() / 3 << '\n';
    printMesh(me);
    std::cout << "Assimp read time: " << elapsed_seconds.count() << '\n';

    std::vector<std::string> args;
    args.push_back("../test/bones2.fbx");
    args.push_back("../test/.format");
    args.push_back("../test/out.mesh");
    args.push_back("-d");

    mesh me1;
    const auto start1{ std::chrono::steady_clock::now() };
    readMeshFile("../test/out.mesh", me1);
    const auto end1{ std::chrono::steady_clock::now() };
    const std::chrono::duration<double> elapsed_seconds1{ end1 - start1 };

    std::cout << "Vert Count: " << me1.verts.size() / 4 << '\n';
    std::cout << "Indices Count: " << me1.indices.size() / 3 << '\n';
    printMesh(me1);
    std::cout << "Binary read time: " << elapsed_seconds1.count() << '\n';

    mesh me2;
    const auto start2{ std::chrono::steady_clock::now() };
    readMeshFile("../test/yeet.mesh", me2);
    const auto end2{ std::chrono::steady_clock::now() };
    const std::chrono::duration<double> elapsed_seconds2{ end2 - start2 };

    std::cout << "Vert Count: " << me2.verts.size() / 4 << '\n';
    std::cout << "Indices Count: " << me2.indices.size() / 3 << '\n';
    printMesh(me2);
    std::cout << "Binary read time: " << elapsed_seconds2.count() << '\n';


    mc::compilationInfo ci_c;
    mc::fileUnit fu;
    fu.c = mc::ctype::per_mesh;
    fu.output_file = "../test/{mesh}_code.txt";
    fu.mode = mc::printMode::plainText;
    fu.preamble.push_back(mc::field::ptr(new mc::cfield(mc::stype::int4, mc::anyType::getValue(mc::stype::int4, -50))));
    fu.preamble.push_back(mc::field::ptr(new mc::pfield(mc::stype::uint4, mc::ptype::buffers_per_unit)));
    fu.preamble.push_back(mc::field::ptr(new mc::pfieldT<unsigned int>(mc::ptype::buffer_size)));
    fu.preamble.push_back(mc::field::ptr(new mc::pfieldT<unsigned int>(mc::ptype::entries_per_unit)));
    fu.preamble.push_back(mc::field::ptr(new mc::pfield(mc::stype::uint4, mc::ptype::entries_per_buffer)));
    fu.preamble.push_back(mc::field::ptr(new mc::pfieldT<unsigned int>(mc::ptype::entry_size)));
    fu.preamble.push_back(mc::field::ptr(new mc::pfieldT<unsigned int>(mc::ptype::fields_per_unit)));
    fu.preamble.push_back(mc::field::ptr(new mc::pfieldT<unsigned int>(mc::ptype::fields_per_buffer)));
    fu.preamble.push_back(mc::field::ptr(new mc::pfieldT<unsigned int>(mc::ptype::fields_per_entry)));
    fu.preamble.push_back(mc::field::ptr(new mc::pfieldT<unsigned int>(mc::ptype::field_size)));
    mc::buffer buff;
    buff.c = mc::ctype::per_vertex;
    buff.preamble.push_back(mc::field::ptr(new mc::cfieldT<int>(-30)));
    buff.preamble.push_back(mc::field::ptr(new mc::bpfield(mc::stype::uint4, mc::ptype::buffer_size)));
    buff.preamble.push_back(mc::field::ptr(new mc::bpfieldT<unsigned int>(mc::ptype::entries_per_buffer)));
    buff.preamble.push_back(mc::field::ptr(new mc::bpfieldT<unsigned int>(mc::ptype::entry_size)));
    buff.preamble.push_back(mc::field::ptr(new mc::bpfieldT<unsigned int>(mc::ptype::fields_per_buffer)));
    buff.preamble.push_back(mc::field::ptr(new mc::bpfieldT<unsigned int>(mc::ptype::fields_per_entry)));
    buff.preamble.push_back(mc::field::ptr(new mc::bpfieldT<unsigned int>(mc::ptype::field_size)));
    buff.fields.push_back(mc::field::ptr(new mc::vfield(mc::stype::float4, mc::vtype::vertex, { 0 })));
    buff.fields.push_back(mc::field::ptr(new mc::vfieldT<float>(mc::vtype::vertex, { 1 })));
    buff.fields.push_back(mc::field::ptr(new mc::cfieldT<int>(-20)));
    fu.buffers.push_back(buff);
    fu.buffers.push_back(buff);
    ci_c.file_units.push_back(fu);

    ci_c.compileFile("../test/cube.obj");

    try {
        mc::compilationInfo ci_j("../test/format.json");
        ci_j.compileFile("../test/cube.obj");
        mc::compilationInfo::units.clear();
    }
    catch (mc::jsonException& je) {
        std::cout << je.what() << std::endl;
    }
    catch (mc::compileException& ce) {
        std::cout << ce.what() << std::endl;
    }
}

#endif // _DEBUG