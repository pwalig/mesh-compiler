#include "case.h"

tests::Case::Case(
    const std::function<mc::compilationInfo()>& getCompilationInfo_,
    const std::vector<std::string>& resultingFiles_
) : getCompilationInfo(getCompilationInfo_), resultingFiles(resultingFiles_) { }

std::function<mc::compilationInfo()> tests::Case::getFromFile(const std::string& fileName)
{
    return [fileName]() { return mc::compilationInfo(fileName); };
}
