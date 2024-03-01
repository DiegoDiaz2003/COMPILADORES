#include <iostream>
#include <fstream>
#include <cctype>
#include <string>
#include <vector>
#include <algorithm> 


using namespace std;

// Definir tipos de token
enum class TokenType {
    KEYWORD, IDENTIFIER, NUMBER, SYMBOL, UNKNOWN
};

// Estructura para representar tokens
struct Token {
    TokenType type;
    string value;
};

// Función para clasificar palabras
TokenType identifyToken(const string& word) {
    // Palabras clave
    const vector<string> keywords = {"if", "else", "while", "return"};

    // Verificar si es una palabra clave
    for (const string& keyword : keywords) {
        if (word == keyword) return TokenType::KEYWORD;
    }

    // Verificar si es un número (simplificación: solo enteros)
    if (all_of(word.begin(), word.end(), ::isdigit)) return TokenType::NUMBER;

    // Verificar si es un símbolo
    if (word.size() == 1 && string("+-*/").find(word) != string::npos) return TokenType::SYMBOL;

    // Por default, es un identificador
    return TokenType::IDENTIFIER;
}

// Función principal
int main() {
    ifstream file("input.txt");
    string word;
    vector<Token> tokens;

    // Leer cada palabra del archivo
    while (file >> word) {
        Token token = {identifyToken(word), word};
        tokens.push_back(token);
    }

    // Imprimir los tokens identificados
    for (const Token& token : tokens) {
        cout << "Token: " << token.value << ", Tipo: ";
        switch (token.type) {
            case TokenType::KEYWORD: cout << "Palabra clave"; break;
            case TokenType::IDENTIFIER: cout << "Identificador"; break;
            case TokenType::NUMBER: cout << "Número"; break;
            case TokenType::SYMBOL: cout << "Símbolo"; break;
            default: cout << "Desconocido"; break;
        }
        cout << endl;
    }

    return 0;
}
