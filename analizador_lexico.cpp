#include <iostream>  //iblioteca estándar de entrada y salida para usar cout.
#include <fstream>   // biblioteca para manejar archivos.
#include <cctype>    // funciones para clasificar caracteres (por ejemplo, isdigit para números).
#include <string>    //biblioteca de manejo de cadenas de texto.
#include <vector>    // biblioteca para usar contenedores vector.
#include <algorithm> //biblioteca de algoritmos, necesaria para all_of.

using namespace std; // Usa el espacio de nombres std para evitar escribir std:: delante de cada elemento de la STL.

// Definir tipos de token
enum class TokenType
{
    KEYWORD,
    IDENTIFIER,
    NUMBER,
    SYMBOL,
    UNKNOWN
}; // Define un tipo enumerado para los distintos tipos de tokens.

// Estructura para representar tokens
struct Token
{
    TokenType type; // Tipo de token según el enum TokenType.
    string value;   // Valor textual del token.
};                  // Define una estructura Token con un tipo y valor.

// Función para clasificar palabras
TokenType identifyToken(const string &word)
{
    // Palabras clave
    const vector<string> keywords = {"auto", "break", "case", "catch", "class", "const", "continue", "default", "delete", "do", "else", "enum", "explicit", "export", "extern", "false", "for", "friend", "goto", "if", "inline", "mutable", "namespace", "new", "operator", "private", "protected", "public", "register", "reinterpret_cast", "return", "sizeof", "static", "static_cast", "struct", "switch", "template", "this", "throw", "true", "try", "typedef", "typeid", "typename", "union", "using", "virtual", "void", "volatile", "while"}; // Lista de palabras clave.

    // Verificar si es una palabra clave
    for (const string &keyword : keywords)
    { // Recorre la lista de palabras clave.
        if (word == keyword)
            return TokenType::KEYWORD; // Si la palabra es una palabra clave, retorna KEYWORD.
    }

    // Verificar si es un número (simplificación: solo enteros)
    if (all_of(word.begin(), word.end(), ::isdigit))
        return TokenType::NUMBER; // Si todos los caracteres son dígitos, retorna NUMBER.

    // Verificar si es un símbolo
    if (word.size() == 1 && string(" << >> + - * / % = == != > < >= <= && || ! & | ^ ~  += -= *= /= %= <<= >>= &= |= ^= ++ -- -> . :: ? : , ; ( ) [ ] { } #").find(word) != string::npos)
        return TokenType::SYMBOL; // Si es un símboloretorna SYMBOL.

    // Por default, es un identificador
    return TokenType::IDENTIFIER; // Si no es ninguna de las anteriores, retorna IDENTIFIER.
}

// Función principal
int main()
{
    ifstream file("input.txt"); // Abre el archivo input.txt para lectura.
    string word;                // Variable para almacenar cada palabra leída del archivo.
    vector<Token> tokens;       // Vector para almacenar los tokens identificados.

    // Leer cada palabra del archivo
    while (file >> word)
    {                                              // leer en el archivo...
        Token token = {identifyToken(word), word}; // Crea un token con el tipo y valor identificados.
        tokens.push_back(token);                   // Añade el token al vector de tokens.
    }

    // Imprimir los tokens identificados
    for (const Token &token : tokens)
    {                                                   // Recorre el vector de tokens.
        cout << "Token: " << token.value << ", Tipo: "; // Imprime el valor del token.
        switch (token.type)
        { // Dependiendo del tipo del token...
        case TokenType::KEYWORD:
            cout << "Palabra clave";
            break; // Imprime "Palabra clave" si es una palabra clave.
        case TokenType::IDENTIFIER:
            cout << "Identificador";
            break; // Imprime "Identificador" si es un identificador.
        case TokenType::NUMBER:
            cout << "Número";
            break; // Imprime "Número" si es un número.
        case TokenType::SYMBOL:
            cout << "Símbolo";
            break; // Imprime "Símbolo" si es un símbolo.
        default:
            cout << "Desconocido";
            break; // Imprime "Desconocido" si no se reconoce el tipo (no debería suceder).
        }
        cout << endl; // Salto de línea después de imprimir cada token.
    }

    return 0; // Termina la ejecución del programa con éxito.
}
