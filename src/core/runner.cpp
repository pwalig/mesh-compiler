#include "runner.h"
#include <iostream>
#include <sstream>
#include "compilation-info.h"
#include "exceptions/jsonException.h"
#include "exceptions/formatException.h"
#include "exceptions/compileException.h"
#include <args.hxx>

const std::string mc::version = "2.2.2";
const std::string mc::programName = "mesh-compiler";

// returns vector of pointers into the string "line" passed as argument
// string must outlive the vector
// modifies the string to contain null characters at the ends of words
std::vector<char*> cArgs(std::string& line) {
    std::vector<char*> res;
    bool word = false;
    for (size_t i = 0; i < line.size(); ++i) {
        if (std::isspace(line[i])) {
            word = false;
            line[i] = '\0';
        }
        else if (word == false) {
            word = true;
            res.push_back(&line[i]);
        }
    }
    return res;
}

void mc::run(int argc, char** argv)
{
    if (argc == 1) {
        std::string line = "";
        while (1) {
            std::cout << "> ";
            std::getline(std::cin, line);
            if (line == "q") return;
            line = mc::programName + " " + line;
            std::vector<char*> args = cArgs(line);
            runOnce((int)args.size(), args.data());
        }
    }
    else runOnce(argc, argv);
}

void mc::runOnce(int argc, char** argv) {

    // parser creation
    args::ArgumentParser parser("3D file converter to a customizable binary file format\nversion " + mc::version,
        "If no arguments passed program will run in interactive mode, type q to quit\nFor full documentation and source code visit: https://github.com/pwalig/mesh-compiler"
    );
    args::HelpFlag help_flag(parser, "help", "Display this help menu", {'h', "help"});
    args::ValueFlag<std::string> format_flag(parser, "format file", "Specifies path to .format file", {'f', "format"});
    args::Flag version_flag(parser, "version", "Display version of this software", { 'v', "version" });
    args::Flag debug_flag(parser, "debug info", "Display debugging information", { 'd', "debug" });
    args::Positional<std::string> source_arg(parser, "source file", "Specifies path to 3D source file");
    args::Positional<std::string> format_arg(parser, "format file", "Specifies path to .format file");
    
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
        std::cerr << e.what() << "\n";
        std::cerr << parser;
        return;
    }
    catch (args::ValidationError e)
    {
        std::cerr << e.what() << "\n";
        std::cerr << parser;
        return;
    }

    // reading parsed values
    if (version_flag) {
        std::cout << mc::version << "\n";
        return;
    }
    std::string sourceFile = "";
    std::string formatFile = "";
    if (source_arg) sourceFile = args::get(source_arg);
    else {
        std::cerr << "source file unspecified\n" << parser;
        return;
    }
    if (format_arg) formatFile = args::get(format_arg);
    if (format_flag) {
        if (formatFile == "") formatFile = args::get(format_arg);
        else {
            std::cerr << "format file specified twice\n" << parser;
            return;
        }
    }

    formatFile = (formatFile == "" ? ".format" : formatFile);

    // running the program 
    try {
        if (debug_flag) std::cout << "reading: " << formatFile << "\n";
        mc::compilationInfo ci_j(formatFile);
        if (debug_flag) std::cout << "format aquired\n";
        ci_j.compileFile(sourceFile, debug_flag);
        mc::compilationInfo::units.clear();
        if (debug_flag) std::cout << "compilation successful\n";
    }
    catch (mc::formatException& fe) {
        std::cerr << fe << "\n";
    }
    catch (mc::jsonException& je) {
        std::cerr << "JSON format interpretation error: " << je.what() << "\n";
    }
    catch (std::runtime_error& e) {
        std::cerr << e.what() << "\n";
    }
}
