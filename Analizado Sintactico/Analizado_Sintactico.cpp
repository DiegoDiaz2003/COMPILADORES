#include <iostream>
#include <fstream>
#include <cctype>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

// Definir tipos de token
enum class TokenType {
    KEYWORD,
    IDENTIFIER,
    NUMBER,
    SYMBOL,
    UNKNOWN
};

// Estructura para representar tokens
struct Token {
    TokenType type;
    string value;
};

// Función para clasificar palabras
TokenType identifyToken(const string &word) {
    // Palabras clave
    const vector<string> keywords = {"auto", "break", "case", "catch", "class", "const", "continue", "default", "delete", "do", "else", "enum", "explicit", "export", "extern", "false", "for", "friend", "goto", "if", "inline", "mutable", "namespace", "new", "operator", "private", "protected", "public", "register", "reinterpret_cast", "return", "sizeof", "static", "static_cast", "struct", "switch", "template", "this", "throw", "true", "try", "typedef", "typeid", "typename", "union", "using", "virtual", "void", "volatile", "while"};

    // Verificar si es una palabra clave
    for (const string &keyword : keywords) {
        if (word == keyword)
            return TokenType::KEYWORD;
    }

    // Verificar si es un número (simplificación: solo enteros)
    if (all_of(word.begin(), word.end(), ::isdigit))
        return TokenType::NUMBER;

    // Verificar si es un símbolo
    if (word.size() == 1 && string("+-*/%()").find(word) != string::npos)
        return TokenType::SYMBOL;

    // Por default, es un identificador
    return TokenType::IDENTIFIER;
}

// Funciones para el analizador sintáctico
bool parseExpression(const vector<Token>& tokens, size_t& pos);
bool parseTerm(const vector<Token>& tokens, size_t& pos);
bool parseFactor(const vector<Token>& tokens, size_t& pos);

bool parseExpression(const vector<Token>& tokens) {
    size_t pos = 0;
    return parseExpression(tokens, pos) && pos == tokens.size(); // Asegurarse de que todos los tokens fueron consumidos
}

bool parseExpression(const vector<Token>& tokens, size_t& pos) {
    if (!parseTerm(tokens, pos))
        return false;
    while (pos < tokens.size() && (tokens[pos].value == "+" || tokens[pos].value == "-")) {
        pos++;
        if (!parseTerm(tokens, pos))
            return false;
    }
    return true;
}

bool parseTerm(const vector<Token>& tokens, size_t& pos) {
    if (!parseFactor(tokens, pos))
        return false;
    while (pos < tokens.size() && (tokens[pos].value == "*" || tokens[pos].value == "/")) {
        pos++;
        if (!parseFactor(tokens, pos))
            return false;
    }
    return true;
}

bool parseFactor(const vector<Token>& tokens, size_t& pos) {
    if (pos < tokens.size() && tokens[pos].type == TokenType::NUMBER) {
        pos++; // Consumir el número
        return true;
    } else if (pos < tokens.size() && tokens[pos].value == "(") {
        pos++; // Consumir '('
        if (!parseExpression(tokens, pos))
            return false;
        if (pos < tokens.size() && tokens[pos].value == ")") {
            pos++; // Consumir ')'
            return true;
        }
        return false;
    }
    return false;
}

// Función principal
int main() {
    ifstream file("prueba.txt");

    // Verificar si el archivo se abrió correctamente
    if (!file.is_open()) {
        cout << "Error al abrir el archivo." << endl;
        return 1; // O gestiona el error de otra manera
    }

    string word;
    vector<Token> tokens;

    // Leer cada palabra del archivo
    while (file >> word) {
        Token token = {identifyToken(word), word};
        tokens.push_back(token);
    }

    // Imprimir los tokens identificados
    for (const Token &token : tokens) {
        cout << "Token: " << token.value << ", Tipo: ";
        switch (token.type) {
        case TokenType::KEYWORD:
            cout << "Palabra clave";
            break;
        case TokenType::IDENTIFIER:
            cout << "Identificador";
            break;
        case TokenType::NUMBER:
            cout << "Número";
            break;
        case TokenType::SYMBOL:
            cout << "Símbolo";
            break;
        default:
            cout << "Desconocido";
            break;
        }
        cout << endl;
    }

    // Analizar sintácticamente la secuencia de tokens
    bool isSyntacticallyCorrect = parseExpression(tokens);

    // Imprimir el resultado del análisis sintáctico
    if (isSyntacticallyCorrect) {
        cout << "La secuencia de tokens es sintácticamente correcta." << endl;
    } else {
        cout << "La secuencia de tokens es sintácticamente incorrecta." << endl;
    }

    return 0;
}
