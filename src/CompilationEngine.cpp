#include "CompilationEngine.hpp"

CompilationEngine::CompilationEngine()
{
}
CompilationEngine::CompilationEngine(const char *input, const char *output)
{
    tokenizer_ = new JackTokenizer(input);
    tokenizer_->Advance();
    symbol_table_ = new SymbolTable();
    vm_writer_ = new VMWriter((string(output) + ".vm").c_str());
    CompileClass();
    vm_writer_->Close();
}
CompilationEngine::~CompilationEngine()
{
    delete tokenizer_;
    delete symbol_table_;
    delete vm_writer_;
}
void CompilationEngine::CompileClassVarDec()
{
    Consume(KeywordType::STATIC | KeywordType::FIELD);
    string type = ConsumeType();
    string name = ConsumeVarName();
    KIND kind = tokenizer_->GetKeyword() == KeywordType::STATIC ? KIND::STATIC : KIND::FIELD;
    symbol_table_->Define(name, type, kind);
    while (Exists(','))
    {
        Consume(',');
        string name = ConsumeVarName();
        symbol_table_->Define(name, type, kind);
    }
    Consume(';');
}
void CompilationEngine::CompileClass()
{
    Consume(KeywordType::CLASS);
    symbol_table_->StartClass(ConsumeClassName());
    Consume('{');
    while (Exists(KeywordType::STATIC | KeywordType::FIELD))
        CompileClassVarDec();
    while (Exists(KeywordType::FUNC_TYPE))
        CompileClassSubroutineDec();
    Consume('}');
}
void CompilationEngine::CompileClassSubroutineDec()
{
    string funcType = Consume(KeywordType::FUNC_TYPE);
    if (Exists(KeywordType::VOID))
    {
        Consume(KeywordType::VOID);
    }
    else
        ConsumeType();
    string funcName = ConsumeSubroutineName();
    symbol_table_->StartSubroutine(funcName, funcType);
    whileIndex = -1;
    ifIndex = -1;
    Consume('(');
    if (symbol_table_->MethodType() == "method")
    {
        symbol_table_->Define("this", KIND::ARG);
    }
    CompileParameterList();
    Consume(')');

    CompileSubRoutineBody();
}
void CompilationEngine::CompileParameterList()
{
    if (Exists(TokenType::KEYWORD) || Exists(TokenType::IDENTIFIER))
    {
        string type = ConsumeType();
        string name = ConsumeVarName();
        symbol_table_->Define(name, type, KIND::ARG);
        while (Exists(','))
        {
            Consume(',');
            string type = ConsumeType();
            string name = ConsumeVarName();
            symbol_table_->Define(name, type, KIND::ARG);
        }
    }
}
void CompilationEngine::CompileSubRoutineBody()
{
    Consume('{');
    while (Exists(KeywordType::VAR))
    {
        CompileVarDec();
    }
    vm_writer_->WriteFunction(symbol_table_->ClassName() + "." + symbol_table_->MethodName(), symbol_table_->VarCount(KIND::VAR));
    if (symbol_table_->MethodType() == "constructor")
    {

        vm_writer_->WritePush("constant", symbol_table_->VarCount(KIND::FIELD));
        vm_writer_->WriteCall("Memory.alloc", 1);
        vm_writer_->WritePop("pointer", 0);
    }
    else if (symbol_table_->MethodType() == "method")
    {

        vm_writer_->WritePush("argument", 0);
        vm_writer_->WritePop("pointer", 0);
    }
    CompileStatements();
    Consume('}');
}
void CompilationEngine::CompileStatements()
{
    while (Exists(KeywordType::STATEMENT))
    {
        switch (tokenizer_->GetKeyword())
        {
        case KeywordType::LET:
            CompileLet();
            break;
        case KeywordType::IF:
            ifIndex++;
            CompileIf();
            break;
        case KeywordType::WHILE:
            whileIndex++;
            CompileWhile();
            break;
        case KeywordType::DO:
            CompileDo();
            break;
        case KeywordType::RETURN:
            CompileReturn();
            break;
        }
    }
}
void CompilationEngine::CompileLet()
{
    Consume(KeywordType::LET);
    string idName = ConsumeVarName();
    if (Exists('['))
    {
        vm_writer_->WritePush(IdentifierToSegment(idName), symbol_table_->IndexOf(idName));
        Consume('[');
        CompileExpression();
        vm_writer_->WriteArithmetic("add");
        Consume(']');
        Consume('=');
        CompileExpression();
        vm_writer_->WritePop("temp", 0);
        vm_writer_->WritePop("pointer", 1);
        vm_writer_->WritePush("temp", 0);
        vm_writer_->WritePop("that", 0);
    }
    else
    {
        Consume('=');
        CompileExpression();
        vm_writer_->WritePop(IdentifierToSegment(idName), symbol_table_->IndexOf(idName));
    }
    Consume(';');
}
string CompilationEngine::IdentifierToSegment(string id)
{
    string segment;
    switch (symbol_table_->KindOf(id))
    {
    case KIND::FIELD:
        segment = "this";
        break;
    case KIND::VAR:
        segment = "local";
        break;
    case KIND::ARG:
        segment = "argument";
        break;
    case KIND::STATIC:
        segment = "static";
        break;
    }
    return segment;
}
void CompilationEngine::CompileIf()
{
    int localIndex = ifIndex;
    Consume(KeywordType::IF);
    Consume('(');
    CompileExpression();
    Consume(')');
    Consume('{');
    vm_writer_->WriteIf("IF_TRUE" + to_string(localIndex));
    vm_writer_->WriteGoto("IF_FALSE" + to_string(localIndex));
    vm_writer_->WriteLabel("IF_TRUE" + to_string(localIndex));
    CompileStatements();

    Consume('}');
    if (Exists(KeywordType::ELSE))
        vm_writer_->WriteGoto("IF_END" + to_string(localIndex));
    vm_writer_->WriteLabel("IF_FALSE" + to_string(localIndex));
    if (Exists(KeywordType::ELSE))
    {

        Consume(KeywordType::ELSE);
        Consume('{');
        CompileStatements();
        Consume('}');
        vm_writer_->WriteLabel("IF_END" + to_string(localIndex));
    }
}
void CompilationEngine::CompileWhile()
{
    int localIndex = whileIndex;
    Consume(KeywordType::WHILE);
    Consume('(');
    vm_writer_->WriteLabel("WHILE_EXP" + to_string(localIndex));
    CompileExpression();
    vm_writer_->WriteArithmetic("not");
    vm_writer_->WriteIf("WHILE_END" + to_string(localIndex));
    Consume(')');
    Consume('{');
    CompileStatements();
    vm_writer_->WriteGoto("WHILE_EXP" + to_string(localIndex));
    vm_writer_->WriteLabel("WHILE_END" + to_string(localIndex));
    Consume('}');
}
void CompilationEngine::CompileDo()
{
    Consume(KeywordType::DO);
    string idName = Consume(TokenType::IDENTIFIER);
    ConsumeFunc(idName);
    vm_writer_->WritePop("temp", 0);
    Consume(';');
}
void CompilationEngine::CompileTerm()
{
    if (Exists(TokenType::STRING_CONST))
    {
        string str_const = Consume(TokenType::STRING_CONST);
        vm_writer_->WritePush("constant", str_const.length());
        vm_writer_->WriteCall("String.new", 1);
        for (int i = 0; i < str_const.length(); i++)
        {
            vm_writer_->WritePush("constant", str_const[i]);
            vm_writer_->WriteCall("String.appendChar", 2);
        }
        return;
    }
    if (Exists(TokenType::INT_CONST))
    {
        vm_writer_->WritePush("constant", stoi(Consume(TokenType::INT_CONST)));
        return;
    }
    if (Exists(KeywordType::KEYWORD_CONST))
    {
        string keyword = Consume(KeywordType::KEYWORD_CONST);
        if (keyword == "true")
        {
            vm_writer_->WritePush("constant", 0);
            vm_writer_->WriteArithmetic("not");
        }
        else if (keyword == "false")
        {
            vm_writer_->WritePush("constant", 0);
        }
        else if (keyword == "this")
        {
            vm_writer_->WritePush("pointer", 0);
        }
        else if (keyword == "null")
        {
            vm_writer_->WritePush("constant", 0);
        }
        return;
    }
    if (Exists(TokenType::IDENTIFIER))
    {
        string idName = ConsumeVarName();
        if (Exists('['))
        {
            vm_writer_->WritePush(IdentifierToSegment(idName), symbol_table_->IndexOf(idName));
            Consume('[');
            CompileExpression();
            vm_writer_->WriteArithmetic("add");
            vm_writer_->WritePop("pointer", 1);
            vm_writer_->WritePush("that", 0);
            Consume(']');
        }
        else if (!ConsumeFunc(idName))
        {
            vm_writer_->WritePush(IdentifierToSegment(idName), symbol_table_->IndexOf(idName));
        }
        return;
    }
    if (Exists('('))
    {
        Consume('(');
        CompileExpression();
        Consume(')');
        return;
    }
    if (Exists('-'))
    {
        Consume('-');
        CompileTerm();
        vm_writer_->WriteArithmetic("neg");
        return;
    }
    if (Exists('~'))
    {
        Consume('~');
        CompileTerm();
        vm_writer_->WriteArithmetic("not");
        return;
    }
}
bool CompilationEngine::ConsumeFunc(string idName)
{
    if (Exists('('))
    {
        Consume('(');
        vm_writer_->WritePush("pointer",
                              0);
        int count = CompileExpressionList() + 1;
        vm_writer_->WriteCall(symbol_table_->ClassName() + "." + idName,
                              count);
        Consume(')');
        return true;
    }
    else if (Exists('.'))
    {
        Consume('.');
        string calleeName = ConsumeSubroutineName();
        string className = symbol_table_->TypeOf(idName);
        Consume('(');
        if (className != "")
        {
            vm_writer_->WritePush(IdentifierToSegment(idName),
                                  symbol_table_->IndexOf(idName));
        }
        int count = CompileExpressionList();
        if (className != "")
        {
            count += 1;
        }
        else
            className = idName;

        vm_writer_->WriteCall(className + "." + calleeName,
                              count);
        Consume(')');
        return true;
    }
    return false;
}
void CompilationEngine::CompileReturn()
{
    Consume(KeywordType::RETURN);
    if (!Exists(';'))
    {
        CompileExpression();
    }
    else
    {
        vm_writer_->WritePush("constant", 0);
    }
    vm_writer_->WriteReturn();
    Consume(';');
}
void CompilationEngine::CompileVarDec()
{
    Consume(KeywordType::VAR);
    string type = ConsumeType();
    string name = ConsumeVarName();
    symbol_table_->Define(name, type, KIND::VAR);
    while (Exists(','))
    {
        Consume(',');
        string name = ConsumeVarName();
        symbol_table_->Define(name, type, KIND::VAR);
    }
    Consume(';');
}
string CompilationEngine::Consume(KeywordType keyword)
{
    if (tokenizer_->GetTokenType() == TokenType::KEYWORD && (keyword & tokenizer_->GetKeyword()))
    {
        string retValue = tokenizer_->GetToken();
        tokenizer_->Advance();
        return retValue;
    }

    throw "Compilation Keyword error";
}
string CompilationEngine::Consume(char symbol)
{
    if (tokenizer_->GetTokenType() == TokenType::SYMBOL && (symbol == tokenizer_->GetSymbol()))
    {
        string retValue = tokenizer_->GetToken();
        tokenizer_->Advance();
        return retValue;
    }

    throw "Compilation Symbol error";
}
string CompilationEngine::Consume(TokenType type)
{
    if (tokenizer_->GetTokenType() == type)
    {
        string retVal = tokenizer_->GetToken();
        tokenizer_->Advance();
        return retVal;
    }
    throw "Compilation error";
}
bool CompilationEngine::Exists(KeywordType token)
{
    return tokenizer_->GetTokenType() == TokenType::KEYWORD && tokenizer_->GetKeyword() & token;
}
bool CompilationEngine::Exists(TokenType type)
{
    return tokenizer_->GetTokenType() == type;
}
bool CompilationEngine::Exists(char symbol)
{
    return tokenizer_->GetTokenType() == TokenType::SYMBOL && tokenizer_->GetSymbol() == symbol;
}
string CompilationEngine::ConsumeType()
{
    if (tokenizer_->GetTokenType() == TokenType::KEYWORD)
    {
        return Consume(KeywordType::BOOLEAN | KeywordType::CHAR | KeywordType::INT);
    }
    else if (tokenizer_->GetTokenType() == TokenType::IDENTIFIER)
    {
        return ConsumeClassName();
    }
    throw "Compilation Type error";
}
string CompilationEngine::ConsumeVarName()
{
    return Consume(TokenType::IDENTIFIER);
}
string CompilationEngine::ConsumeClassName()
{
    return Consume(TokenType::IDENTIFIER);
}
string CompilationEngine::ConsumeSubroutineName()
{
    return Consume(TokenType::IDENTIFIER);
}
bool CompilationEngine::IsOp(char symbol)
{
    string ops = "+-*/&|<>=";
    return ops.find(symbol) != string::npos;
}
bool CompilationEngine::IsUnaryOp(char symbol)
{
    string ops = "-~";
    return ops.find(symbol) != string::npos;
}
int CompilationEngine::CompileExpressionList()
{
    int count = 0;
    if (!Exists(')'))
    {
        count++;
        CompileExpression();
        while (Exists(','))
        {
            Consume(',');
            count++;
            CompileExpression();
        }
    }
    return count;
}
void CompilationEngine::CompileExpression()
{
    CompileTerm();
    while (tokenizer_->GetTokenType() == TokenType::SYMBOL && IsOp(tokenizer_->GetSymbol()))
    {
        string op = Consume(tokenizer_->GetSymbol());
        CompileTerm();
        string cmd = "";
        switch (op[0])
        {
        case '+':
            vm_writer_->WriteArithmetic("add");
            break;
        case '-':
            vm_writer_->WriteArithmetic("sub");
            break;
        case '~':
            vm_writer_->WriteArithmetic("not");
            break;
        case '=':
            vm_writer_->WriteArithmetic("eq");
            break;
        case '>':
            vm_writer_->WriteArithmetic("gt");
            break;
        case '<':
            vm_writer_->WriteArithmetic("lt");
            break;
        case '&':
            vm_writer_->WriteArithmetic("and");
            break;
        case '|':
            vm_writer_->WriteArithmetic("or");
            break;
        case '*':
            vm_writer_->WriteCall("Math.multiply", 2);
            break;
        case '/':
            vm_writer_->WriteCall("Math.divide", 2);
            break;
        }
    }
}