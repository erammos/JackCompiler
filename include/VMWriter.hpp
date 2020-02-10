#pragma once
#include <string>
#include <fstream>
using namespace std;
class VMWriter
{
private:
 ofstream output_;
public:
    VMWriter(const char *output);
    void WritePush(string segment, int index);
    void WritePop(string segment, int index);
    void WriteArithmetic(string op);
    void WriteLabel(string label);
    void WriteGoto(string label);
    void WriteIf(string label);
    void WriteCall(string name, int numArgs);
    void WriteFunction(string name, int numLocals);
    void WriteReturn();
    void Close();
};