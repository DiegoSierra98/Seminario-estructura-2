#include <iostream>
#include <vector>
#include <cctype>
#include <string>
#include <unordered_map>

using namespace std;

enum TokenType {
    IDENTIFIER, INTEGER, REAL, STRING, TYPE, OPSUMA, OPMUL, OPRELAC, OPOR, OPAND,
    OPNOT, OPIGUALDAD, SEMICOLON, COMMA, LPAREN, RPAREN, LBRACE, RBRACE, ASSIGN,
    IF, WHILE, RETURN, ELSE, END
};

struct Token {
    string value;
    TokenType type;
};

unordered_map<string, TokenType> keywords = {
    {"if", IF}, {"while", WHILE}, {"return", RETURN}, {"else", ELSE},
    {"int", TYPE}, {"float", TYPE}
};

unordered_map<string, TokenType> operators = {
    {"+", OPSUMA}, {"-", OPSUMA}, {"*", OPMUL}, {"/", OPMUL},
    {"=", ASSIGN}, {"<", OPRELAC}, {">", OPRELAC}, {"<=", OPRELAC},
    {">=", OPRELAC}, {"!=", OPIGUALDAD}, {"==", OPIGUALDAD},
    {"&&", OPAND}, {"||", OPOR}, {"!", OPNOT}
};

unordered_map<char, TokenType> symbols = {
    {';', SEMICOLON}, {',', COMMA}, {'(', LPAREN}, {')', RPAREN},
    {'{', LBRACE}, {'}', RBRACE}
};

class Lexer {
private:
    string input;
    vector<Token> tokens;
    size_t index;
    char peek() { return index < input.size() ? input[index] : '\0'; }
    char advance() { return index < input.size() ? input[index++] : '\0'; }

    void addToken(const string& value, TokenType type) {
        tokens.push_back({ value, type });
    }

public:
    Lexer(string input) : input(input), index(0) {}
    vector<Token> analyze() {
        while (index < input.length()) {
            char current = advance();

            if (isspace(current)) continue;
            if (isalpha(current)) {
                string identifier(1, current);
                while (isalnum(peek())) identifier += advance();
                addToken(identifier, keywords.count(identifier) ? keywords[identifier] : IDENTIFIER);
            }
            else if (isdigit(current)) {
                string number(1, current);
                while (isdigit(peek())) number += advance();
                if (peek() == '.') {
                    number += advance();
                    while (isdigit(peek())) number += advance();
                    addToken(number, REAL);
                }
                else {
                    addToken(number, INTEGER);
                }
            }
            else if (operators.count(string(1, current))) {
                string op(1, current);
                if (operators.count(op + peek())) {
                    op += advance();
                }
                addToken(op, operators[op]);
            }
            else if (symbols.count(current)) {
                addToken(string(1, current), symbols[current]);
            }
            else {
                addToken(string(1, current), END);
            }
        }
        return tokens;
    }
};

void printTokens(const vector<Token>& tokens) {
    cout << "\nToken\t\tTipo" << endl;
    cout << "---------------------" << endl;
    for (const auto& token : tokens) {
        cout << token.value << "\t\t" << token.type << endl;
    }
}

int main() {
    string input;
    cout << "Ingresa la cadena a analizar: ";
    getline(cin, input);

    Lexer lexer(input);
    vector<Token> tokens = lexer.analyze();
    printTokens(tokens);

    return 0;
}
