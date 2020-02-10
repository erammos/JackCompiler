#include "VMWriter.hpp"
VMWriter::VMWriter(const char *output)
{
    output_.open(output);
}
void VMWriter::Close()
{
    output_.close();
}
void VMWriter::WritePush(string segment, int index)
{
    output_ << "push"
            << " " << segment << " " << to_string(index) << endl;
}
void VMWriter::WritePop(string segment, int index)
{
    output_ << "pop"
            << " " << segment << " " << to_string(index) << endl;
}
void VMWriter::WriteArithmetic(string op)
{
    output_ << op << endl;
}
void VMWriter::WriteLabel(string label)
{
    output_ << "label"
            << " " << label << endl;
}
void VMWriter::WriteGoto(string label)
{
    output_ << "goto"
            << " " << label << endl;
}
void VMWriter::WriteIf(string label)
{
    output_ << "if-goto"
            << " " << label << endl;
}
void VMWriter::WriteCall(string name, int numArgs)
{
    output_ << "call"
            << " " << name << " " << to_string(numArgs) << endl;
}
void VMWriter::WriteFunction(string name, int numLocals)
{
    output_ << "function"
            << " " << name << " " << to_string(numLocals) << endl;
}
void VMWriter::WriteReturn()
{
    output_ << "return" << endl;
}