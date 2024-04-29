#include <iostream>
#include <fstream>
#include <cctype>
#include <string>
#include <vector>
#include "COMPILADORES/Analizador_Sintactico/Analizador_Sintactico.cpp"// Incluye el archivo de encabezado del analizador sintáctico

using namespace std;

// Función para realizar análisis semántico
void analizarSemantica(const vector<Token> &tokens)
{
    // Implementación del análisis semántico
    // Aquí puedes agregar la lógica para realizar el análisis semántico basado en los tokens procesados
    // Puedes verificar reglas semánticas, tipos de datos, asignaciones, etc.
}

// Función principal
int main()
{
    ifstream file("archivo.txt");

    // Verificar si el archivo se abrió correctamente
    if (!file.is_open())
    {
        cout << "Error al abrir el archivo." << endl;
        return 1;
    }

    string word;
    vector<Token> tokens;

    // Leer cada palabra del archivo y procesar los tokens
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

    // Realizar análisis sintáctico
    bool isSyntacticallyCorrect = parseExpression(tokens);

    // Imprimir el resultado del análisis sintáctico
    if (isSyntacticallyCorrect)
    {
        cout << "La secuencia de tokens es sintácticamente correcta." << endl;
    }
    else
    {
        cout << "La secuencia de tokens es sintácticamente incorrecta." << endl;
    }

    // Realizar análisis semántico
    analizarSemantica(tokens);

    return 0;
}



