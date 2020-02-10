#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <vector>
using namespace std;
enum class TokenType
{
    KEYWORD,
    SYMBOL,
    IDENTIFIER,
    INT_CONST,
    STRING_CONST
};
enum KeywordType
{
    CLASS = 1 << 0,
    METHOD = 1 << 1,
    FUNCTION = 1 << 2,
    CONSTRUCTOR = 1 << 3,
    BOOLEAN = 1 << 4,
    CHAR = 1 << 5,
    VOID = 1 << 6,
    VAR = 1 << 7,
    STATIC = 1 << 8,
    FIELD = 1 << 9,
    LET = 1 << 10,
    DO = 1 << 11,
    IF = 1 << 12,
    ELSE = 1 << 13,
    WHILE = 1 << 14,
    RETURN = 1 << 15,
    TRUE = 1 << 16,
    FALSE = 1 << 17,
    NULL0 = 1 << 18,
    THIS = 1 << 19,
    INT = 1 << 20,
    KEYWORD_CONST = TRUE | FALSE | NULL0 | THIS,
    FUNC_TYPE = CONSTRUCTOR | FUNCTION | METHOD,
    STATEMENT = LET | IF | WHILE | DO | RETURN
};
inline KeywordType operator|(KeywordType a, KeywordType b)
{
    return static_cast<KeywordType>(static_cast<int>(a) | static_cast<int>(b));
}
class JackTokenizer
{

    std::unordered_map<std::string, KeywordType> keywords_map_ =
        {{"class", KeywordType::CLASS},
         {"method", KeywordType::METHOD},
         {"function", KeywordType::FUNCTION},
         {"constructor", KeywordType::CONSTRUCTOR},
         {"int", KeywordType::INT},
         {"boolean", KeywordType::BOOLEAN},
         {"char", KeywordType::CHAR},
         {"void", KeywordType::VOID},
         {"var", KeywordType::VAR},
         {"static", KeywordType::STATIC},
         {"field", KeywordType::FIELD},
         {"let", KeywordType::LET},
         {"do", KeywordType::DO},
         {"if", KeywordType::IF},
         {"else", KeywordType::ELSE},
         {"while", KeywordType::WHILE},
         {"return", KeywordType::RETURN},
         {"true", KeywordType::TRUE},
         {"false", KeywordType::FALSE},
         {"null", KeywordType::NULL0},
         {"this", KeywordType::THIS}};

    string RemoveComments(string line)
    {
        int npos = line.find("//");
        if (npos > 0 || npos == string::npos)
        {
            return line.substr(0, npos);
        }

        return "";
    }
    string Trim(string line)
    {
        string trimmedLine = "";
        for (int i = 0; i < line.size(); i++)
        {
            if (line[i] != '\r' && line[i] != '\n' && line[i] != '\t')
            {
                trimmedLine += line[i];
            }
        }

        return trimmedLine;
    }
    string RemoveMoreComments(string text)
    {
        int i = text.find("/*");
        while (i != string::npos)
        {
            int j = text.find("*/", i);
            text.erase(i, j - i + 2);
            i = text.find("/*");
        }
        return text;
    }
    bool IsSymbol(char letter)
    {
        string symbols = "{}()[].,;+-*/&|<>=~";
        return symbols.find(letter) != string::npos;
    }
    bool IsSymbol(string token)
    {
        return token.length() == 1 && IsSymbol(token[0]);
    }

    bool IsKeyword(string &token)
    {
        return keywords_map_.find(token) != keywords_map_.end();
    }
    bool IsStringConst(string &token)
    {
        return token[0] == '"';
    }
    bool IsIntConst(const std::string &s)
    {
        std::string::const_iterator it = s.begin();
        while (it != s.end() && std::isdigit(*it))
            ++it;
        return !s.empty() && it == s.end();
    }

private:
    int cur_token_i_ = 0;
    TokenType token_type_;
    KeywordType keyword_;
    string string_val_;
    int int_val_;
    char symbol_;
    vector<string> tokens_;
    string identifier_;
    string token_;

public:
    JackTokenizer();
    JackTokenizer(const char *filename);
    ~JackTokenizer();
    bool HasMoreTokens();
    void Advance();
    TokenType GetTokenType();
    KeywordType GetKeyword();
    char GetSymbol();
    string GetIdentifier();
    int GetIntVal();
    string GetStringVal();
    string GetToken();
};