#include "runner.h"
#include <iostream>
#include <sstream>
#include "compilation-info.h"
#include "exceptions/jsonException.h"
#include "exceptions/formatException.h"
#include "exceptions/compileException.h"


const std::string mc::version = "2.2.0";

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
    else {
        std::vector<std::string> args;
        for (int i = 1; i < argc; ++i) {
            args.push_back(argv[i]);
        }
        runOnce(args);
    }
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

