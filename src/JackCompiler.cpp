#include <iostream>
#include "JackTokenizer.hpp"
#include "CompilationEngine.hpp"
#include "experimental/filesystem"
namespace fs = std::experimental::filesystem;
int main(int argc, char *argv[])
{
    fs::path mPath(argv[1]);
    if (fs::is_directory(mPath))
    {
        for (auto &p : fs::directory_iterator(argv[1]))
        {
            if (p.path().extension() == ".jack")
            {
                CompilationEngine(p.path().c_str(), ((string)p.path().parent_path() + "/" + (string)p.path().stem()).c_str());
            }
        }
    }
    else
    {
       CompilationEngine(mPath.c_str(), ((string)mPath.parent_path() + "/" + (string)mPath.stem()).c_str());
    }
    return 0;
}
