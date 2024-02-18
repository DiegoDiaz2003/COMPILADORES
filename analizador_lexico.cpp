#include <bits/stdc++.h>
using namespace std;

// Definición de vectores que contienen palabras clave, operadores, números, paréntesis y corchetes
vector<string> demo = {
    "auto",   "break",   "case",  "char",     "const",  "continue", "default",
    "do",     "double",  "else",  "enum",     "extern", "float",    "for",
    "goto",   "if",      "int",   "signed",   "sizeof", "static",   "struct",
    "switch", "typedef", "union", "unsigned", "void",   "volatile", "while"};

vector<string> hello = {"-", "*", "=", "+", "/"};

vector<string> ten = {"67", "87", "5", "12", "90"};

vector<string> parenthesis = {"(", ")"};

vector<string> brackets = {"[", "]"};

// Función para imprimir el tipo de token de una cadena dada
void printout(string q) {
  // Manejar caso de cadena vacía
  if (q.empty()) return;

  // Determinar el tipo de token y mostrarlo en la salida estándar
  if (find(demo.begin(), demo.end(), q) != demo.end())
    cout << q << " \t keyword\n";
  else if (find(hello.begin(), hello.end(), q) != hello.end())
    cout << q << " \t operator\n";
  else if (find(ten.begin(), ten.end(), q) != ten.end())
    cout << q << " \t number\n";
  else if (find(parenthesis.begin(), parenthesis.end(), q) != parenthesis.end())
    cout << q << " \t parenthesis\n";
  else if (find(brackets.begin(), brackets.end(), q) != brackets.end())
    cout << q << " \t separator\n"; // Corregir el término "separator"
  else
    cout << q << " \t unknown\n"; // Manejar casos desconocidos
}

int main() {
  string line;
  vector<string> sample;

  // Leer la entrada estándar y dividirla en tokens separados por espacios
  while (getline(cin, line, ' ')) {
    sample.push_back(line);
  }
  
  // Iterar sobre los tokens y determinar su tipo
  for (auto q : sample) printout(q);

  return 0;
}
