#pragma once
#include <unordered_map>
#include <string>
using namespace std;

enum class KIND
{
    STATIC,
    FIELD,
    ARG,
    VAR
};
struct Info
{
    string name;
    string type;
    KIND kind;
    int count;
};
class SymbolTable
{
private:
    unordered_map<string, Info> globalTable_ = {};
    unordered_map<string, Info> localTable_ = {};
    unordered_map<KIND, int> globalCounter_ = {};
    unordered_map<KIND, int> localCounter_ = {};
    void InternalDefine(unordered_map<KIND, int> &counter, unordered_map<string, Info> &table, Info &info);
    unordered_map<KIND, int> GetCounter(KIND kind);
    unordered_map<string, Info> GetTable(KIND kind);
    string method_name_;
    string class_name_;
    string method_type_;
    bool GetInfo(string name, Info &);

public:
    SymbolTable();
    void StartSubroutine(string name, string type);
    void StartClass(string name);
    void Define(string name, string type, KIND kind);
    void Define(string name, KIND kind);
    int VarCount(KIND kind);
    KIND KindOf(string name);
    string TypeOf(string name);
    int IndexOf(string name);
    string MethodName();
    string MethodType();
    string ClassName();
};