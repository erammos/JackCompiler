#pragma once
#include "SymbolTable.hpp"
#include "JackTokenizer.hpp"
#include "VMWriter.hpp"
class CompilationEngine
{
private:
    JackTokenizer *tokenizer_;
    SymbolTable *symbol_table_;
    VMWriter *vm_writer_;
    string Consume(TokenType type);
    string Consume(KeywordType keyword);
    string Consume(char symbol);
    bool Exists(KeywordType token);
    bool Exists(TokenType type);
    bool Exists(char symbol);
    string ConsumeType();
    string ConsumeVarName();
    string ConsumeClassName();
    string ConsumeSubroutineName();
    bool IsOp(char symbol);
    bool IsUnaryOp(char symbol);
    bool ConsumeFunc(string idName);
    string IdentifierToSegment(string id);
    int whileIndex = -1;
    int ifIndex = -1;
public:
    CompilationEngine();
    CompilationEngine(const char *input, const char *output);
    ~CompilationEngine();
    void CompileClass();
    void CompileClassVarDec();
    void CompileClassSubroutineDec();
    void CompileParameterList();
    void CompileSubRoutineBody();
    void CompileVarDec();
    void CompileStatements();
    void CompileLet();
    void CompileIf();
    void CompileWhile();
    void CompileDo();
    void CompileReturn();
    void CompileTerm();
    int CompileExpressionList();
    void CompileExpression();
};