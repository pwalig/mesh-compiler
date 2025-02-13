#include "runner.h"
#include <iostream>
#include <sstream>
#include "compilation-info.h"
#include "exceptions/jsonException.h"
#include "exceptions/formatException.h"
#include "exceptions/compileException.h"
#include <args.hxx>

const std::string mc::version = "2.2.0";
const std::string mc::programName = "mesh-compiler";

void mc::run(int argc, char** argv)
{
    if (argc == 1) {
        std::string line = "";
        while (1) {
            std::cout << "> ";
            std::getline(std::cin, line);
            if (line == "q") return;
            std::vector<std::string> args;
            std::stringstream ss(line);
            while (ss) {
                std::string a;
                ss >> a;
                if (!a.empty()) args.push_back(a);
            }
            runOnce(args);
        }
    }
    else runOnce(argc, argv);
}

std::vector<const char*> cArgs(const std::vector<std::string>& args)
{
    std::vector<const char*> res(args.size());
    for (const std::string& str : args) {
        res.push_back(str.data());
    }
    return res;
}

void mc::runOnce(const std::vector<std::string>& args)
{
    if (args.size() == 1 && (args[0] == "-v" || args[0] == "--version")) {
        std::cout << mc::version << std::endl;
        return;
    }
    try {
        mc::compilationInfo ci_j(args.size() >= 2 ? args[1] : ".format");
        ci_j.compileFile(args[0]);
        mc::compilationInfo::units.clear();
    }
    catch (std::runtime_error& e) {
        std::cout << e.what() << "\n";
    }
}

void mc::runOnce(int argc, char** argv) {

    // parser creation
    args::ArgumentParser parser("program for conversion of 3D files");
    args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
    args::ValueFlag<std::string> format_flag(parser, "format file", "specifies path to .format file", {'f', "format"});
    args::Flag version(parser, "version", "Display version of this software", { 'v', "version" });
    args::Positional<std::string> source_file(parser, "source", "specifies path to 3D source file");
    args::Positional<std::string> format_file(parser, "format", "specifies path to .format file");
    
    // parsing
    try
    {
        parser.ParseCLI(argc, argv);
    }
    catch (args::Help)
    {
        std::cout << parser;
        return;
    }
    catch (args::ParseError e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return;
    }
    catch (args::ValidationError e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return;
    }

    // reading parsed values
    if (version) {
        std::cout << version << "\n";
        return;
    }
    std::string sourceFile = "";
    std::string formatFile = "";
    if (source_file) sourceFile = args::get(source_file);
    if (format_file) formatFile = args::get(format_file);
    if (format_flag) {
        if (formatFile == "") formatFile = args::get(format_file);
        else {
            std::cerr << "format file specified twice\n" << parser;
            return;
        }
    }

    // running the program 
    try {
        mc::compilationInfo ci_j(formatFile);
        ci_j.compileFile(sourceFile);
        mc::compilationInfo::units.clear();
    }
    catch (std::runtime_error& e) {
        std::cout << e.what() << "\n";
    }
}
