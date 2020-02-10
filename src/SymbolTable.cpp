#include "SymbolTable.hpp";
SymbolTable::SymbolTable()
{
}
void SymbolTable::StartSubroutine(string name, string type)
{
    method_name_ = name;
    method_type_ = type;
    localTable_.clear();
    localCounter_.clear();
}
void SymbolTable::StartClass(string name)
{
    class_name_ = name;
    globalTable_.clear();
    globalCounter_.clear();
}
string SymbolTable::MethodName()
{
    return method_name_;
}
string SymbolTable::MethodType()
{
    return method_type_;
}
string SymbolTable::ClassName()
{
    return class_name_;
}
void SymbolTable::Define(string name, KIND kind)
{
     Define(name, class_name_, kind);
}
void SymbolTable::Define(string name, string type, KIND kind)
{
    Info info;
    info.kind = kind;
    info.name = name;
    info.type = type;
    if (kind == KIND::ARG || kind == KIND::VAR)
    {
         InternalDefine(localCounter_, localTable_, info);
    }
    else
    {
         InternalDefine(globalCounter_, globalTable_, info);
    }
}
int SymbolTable::VarCount(KIND kind)
{
    return GetCounter(kind)[kind];
}
KIND SymbolTable::KindOf(string name)
{
    Info info;
    if (GetInfo(name, info))
    {
        return info.kind;
    }
}
string SymbolTable::TypeOf(string name)
{
    Info info;
    if (GetInfo(name, info))
    {
        return info.type;
    }
    return "";
}
int SymbolTable::IndexOf(string name)
{
    Info info;
    if (GetInfo(name, info))
    {
        return info.count;
    }
    return -1;
}
unordered_map<string, Info> SymbolTable::GetTable(KIND kind)
{
    return (kind == KIND::ARG || kind == KIND::VAR) ? localTable_ : globalTable_;
}
unordered_map<KIND, int> SymbolTable::GetCounter(KIND kind)
{
    return (kind == KIND::ARG || kind == KIND::VAR) ? localCounter_ : globalCounter_;
}
bool SymbolTable::GetInfo(string name, Info &info)
{
    if (localTable_.find(name) != localTable_.end())
    {
        info = localTable_[name];
        return true;
    }
    else if (globalTable_.find(name) != globalTable_.end())
    {
        info = globalTable_[name];
        return true;
    }
    else
        return false;
}
void SymbolTable::InternalDefine(unordered_map<KIND, int> &counter, unordered_map<string, Info> &table, Info &info)
{
    if (table.find(info.name) != table.end())
        return;
    if (counter.find(info.kind) != counter.end())
    {
        info.count = counter[info.kind];
    }
    else
    {
        counter[info.kind] = 0;
        info.count = 0;
    }
    counter[info.kind]++;
    table[info.name] = info;
}
