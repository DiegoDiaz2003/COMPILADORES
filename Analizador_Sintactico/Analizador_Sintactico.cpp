#include <iostream>
#include <fstream>
#include <cctype>
#include <string>
#include <vector>
#include <algorithm>


using namespace std;

// Definir tipos de token
enum class TokenType
{
    KEYWORD,
    IDENTIFIER,
    NUMBER,
    SYMBOL,
    UNKNOWN
};

// Estructura para representar tokens
struct Token
{
    TokenType type;
    string value;
};

// Función para clasificar palabras
TokenType identifyToken(const string &word)
{
    // Palabras clave
    const vector<string> keywords = {"auto", "break", "case", "catch", "class",
     "const", "continue", "default", "delete", "do", "else", "enum", "explicit", 
     "export", "extern", "false", "for", "friend", "goto", "if", "inline", "mutable", 
     "namespace", "new", "operator", "private", "protected", "public", "register",
      "reinterpret_cast", "return", "sizeof", "static", "static_cast", "struct",
       "switch", "template", "this", "throw", "true", "try", "typedef", "typeid", 
       "typename", "union", "using", "virtual", "void", "volatile", "while"};

    // Verificar si es una palabra clave
    for (const string &keyword : keywords)
    {
        if (word == keyword)
            return TokenType::KEYWORD;
    }

    // Verificar si es un número (simplificación: solo enteros)
    if (all_of(word.begin(), word.end(), ::isdigit))
        return TokenType::NUMBER;

    // Verificar si es un símbolo
    if (word.size() == 1 && string("+-*/%(){}").find(word) != string::npos)
        return TokenType::SYMBOL;

    // Por default, es un identificador
    return TokenType::IDENTIFIER;
}

// Funciones para el analizador sintáctico
bool parseExpression(const vector<Token> &tokens, size_t &pos); // Analiza y evalúa expresiones matemáticas complejas con múltiples términos y operadores.
bool parseTerm(const vector<Token> &tokens, size_t &pos); //Analiza y evalúa términos individuales en una expresión, generalmente implicando multiplicación o división.
bool parseFactor(const vector<Token> &tokens, size_t &pos); //Analiza y evalúa factores dentro de un término, que pueden ser operandos simples o subexpresiones dentro de paréntesis.
bool parseUnary(const vector<Token> &tokens, size_t &pos);// Analiza valores binarios

bool parseExpression(const vector<Token> &tokens)
{
    size_t pos = 0; // Inicializa la posición de análisis en 0.
    // Llama a la función sobrecargada parseExpression y verifica si todos los tokens fueron consumidos.
    bool status = parseExpression(tokens, pos) && pos == tokens.size();
    // Imprime un mensaje de error si la secuencia es sintácticamente incorrecta.
    if (!status && pos < tokens.size()) {
        cout << "Error sintáctico cerca de: " << tokens[pos].value << endl;
    }
    return status; // Retorna el estado del análisis.
}

bool parseExpression(const vector<Token> &tokens, size_t &pos)
{
    // Intenta analizar un término. Si falla, la expresión es inválida.
    if (!parseTerm(tokens, pos))
        return false;
    // Mientras encuentre operadores de suma o resta, intenta analizar otro término.
    while (pos < tokens.size() && (tokens[pos].value == "+" || tokens[pos].value == "-"))
    {
        pos++; // Avanza al siguiente token.
        if (!parseTerm(tokens, pos)) // Intenta analizar el siguiente término.
            return false;
    }
    return true; // Retorna verdadero si la expresión es válida.
}

bool parseTerm(const vector<Token> &tokens, size_t &pos)
{
    // Intenta analizar un factor. Si falla, el término es inválido.
    if (!parseUnary(tokens, pos))
        return false;
    // Mientras encuentre operadores de multiplicación o división, intenta analizar otro factor.
    while (pos < tokens.size() && (tokens[pos].value == "*" || tokens[pos].value == "/"))
    {
        pos++; // Avanza al siguiente token.
        if (!parseUnary(tokens, pos)) // Intenta analizar el siguiente factor.
            return false;
    }
    return true; // Retorna verdadero si el término es válido.
}

bool parseUnary(const vector<Token> &tokens, size_t &pos)
{
    // Si encuentra un operador unario '-', avanza al siguiente token.
    if (pos < tokens.size() && tokens[pos].value == "-")
    {
        pos++; // Consumir el operador unario.
    }
    // Llama a parseFactor para analizar el factor que sigue al operador unario.
    return parseFactor(tokens, pos);
}

bool parseFactor(const vector<Token> &tokens, size_t &pos)
{
    // Si el token actual es un número, simplemente avanza al siguiente token.
    if (pos < tokens.size() && tokens[pos].type == TokenType::NUMBER)
    {
        pos++; // Consumir el número.
        return true;
    }
    // Si el token actual es '(', intenta analizar una expresión entre paréntesis.
    else if (pos < tokens.size() && tokens[pos].value == "(")
    {
        pos++; // Consumir '('.
        if (!parseExpression(tokens, pos)) // Analiza la expresión dentro de los paréntesis.
            return false;
        if (pos < tokens.size() && tokens[pos].value == ")")
        {
            pos++; // Consumir ')'.
            return true;
        }
        return false; // Retorna falso si no encuentra el paréntesis de cierre.
    }
    return false; // Retorna falso si el factor no es un número ni una expresión entre paréntesis.
}


// Función principal
int main()
{
    ifstream file("prueba.txt");

    // Verificar si el archivo se abrió correctamente
    if (!file.is_open())
    {
        cout << "Error al abrir el archivo." << endl;
        return 1; // O gestiona el error de otra manera
    }

    string word;
    vector<Token> tokens;

    // Leer cada palabra del archivo
    while (file >> word)
    {
        Token token = {identifyToken(word), word};
        tokens.push_back(token);
    }

    // Imprimir los tokens identificados
    for (const Token &token : tokens)
    {
        cout << "Token: " << token.value << ", Tipo: ";
        switch (token.type)
        {
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
    if (isSyntacticallyCorrect)
    {
        cout << "La secuencia de tokens es sintácticamente correcta." << endl;
    }
    else
    {
        cout << "La secuencia de tokens es sintácticamente correcta, pero hay algunos errores" << endl;
    }

    return 0;
}
