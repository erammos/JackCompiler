#include "JackTokenizer.hpp"

JackTokenizer::JackTokenizer()
{
}

JackTokenizer::~JackTokenizer()
{
}
JackTokenizer::JackTokenizer(const char *filename)
{
    ifstream in(filename);
    std::string line;
    string loaded_text = "";
    while (std::getline(in, line))
    {
        if (!line.empty())
        {
            string cmd = RemoveComments(Trim(line));
            if (cmd != "")
            {
                loaded_text += cmd;
            }
        }
    }

    loaded_text = RemoveMoreComments(loaded_text);

    int i = 0;
    while (i < loaded_text.size())
    {
        if (isspace(loaded_text[i]))
        {
            i++;
            continue;
        }

        if (IsSymbol(loaded_text[i]))
        {
            tokens_.push_back(loaded_text.substr(i, 1));
            i++;
        }
        else if (loaded_text[i] == '"')
        {
            int j = i + 1;
            while (j < loaded_text.size() && loaded_text[j] != '"')
                j++;
            tokens_.push_back(loaded_text.substr(i, j - i));
            i = j + 1;
        }
        else
        {
            int j = i + 1;
            while (j < loaded_text.size() && !IsSymbol(loaded_text[j]) && !isspace(loaded_text[j]))
                j++;
            tokens_.push_back(loaded_text.substr(i, j - i));
            i = j;
        }
    }
    in.close();
}
void JackTokenizer::Advance()
{
    if (!HasMoreTokens())
        return;
    string token = tokens_[cur_token_i_];
    if (IsKeyword(token))
    {
        token_type_ = TokenType::KEYWORD;
        keyword_ = keywords_map_[token];
        token_ = token;
    }
    else if (IsSymbol(token))
    {
        token_type_ = TokenType::SYMBOL;
        symbol_ = token[0];
        token_ = token;
    }
    else if (IsIntConst(token))
    {
        token_type_ = TokenType::INT_CONST;
        int_val_ = stoi(token);
        token_ = token;
    }
    else if (IsStringConst(token))
    {
        token_type_ = TokenType::STRING_CONST;
        string_val_ = token.substr(1);
        token_ = string_val_;
    }
    else
    {
        token_type_ = TokenType::IDENTIFIER;
        identifier_ = token;
        token_ = token;
    }
    cur_token_i_++;
};
TokenType JackTokenizer::GetTokenType()
{
    return token_type_;
}
string JackTokenizer::GetToken()
{
    return token_;
}
KeywordType JackTokenizer::GetKeyword()
{
    return keyword_;
}
string JackTokenizer::GetStringVal()
{
    return string_val_;
}
int JackTokenizer::GetIntVal()
{
    return int_val_;
}
char JackTokenizer::GetSymbol()
{
    return symbol_;
}
string JackTokenizer::GetIdentifier()
{
    return identifier_;
}
bool JackTokenizer::HasMoreTokens()
{
    return cur_token_i_ < tokens_.size();
}