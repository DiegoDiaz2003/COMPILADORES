#include <iostream>
#include <fstream>
#include <cctype>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include <iomanip>

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
    const vector<string> keywords = {
        "auto", "break", "case", "catch", "class", "const", "continue", "default", "delete",
        "do", "else", "enum", "explicit", "export", "extern", "false", "for", "friend", "goto",
        "if", "inline", "mutable", "namespace", "new", "operator", "private", "protected", "public",
        "register", "reinterpret_cast", "return", "sizeof", "static", "static_cast", "struct", "switch",
        "template", "this", "throw", "true", "try", "typedef", "typeid", "typename", "union", "using",
        "virtual", "void", "volatile", "while", "int", "string"
    };

    // Verificar si es una palabra clave
    if (find(keywords.begin(), keywords.end(), word) != keywords.end()) {
        return TokenType::KEYWORD;
    }

    // Verificar si es un número (simplificación: solo enteros)
    if (all_of(word.begin(), word.end(), ::isdigit)) {
        return TokenType::NUMBER;
    }

    // Verificar si es un símbolo
    if (word.size() == 1 && string("+-*/%(){}=;").find(word) != string::npos) {
        return TokenType::SYMBOL;
    }

    // Por default, es un identificador
    return TokenType::IDENTIFIER;
}

// Función para separar tokens considerando símbolos individuales
vector<Token> tokenize(const string &line) {
    vector<Token> tokens;
    string word;
    for (char ch : line) {
        if (isspace(ch)) {
            if (!word.empty()) {
                tokens.push_back({identifyToken(word), word});
                word.clear();
            }
        } else if (string("+-*/%(){}=;").find(ch) != string::npos) {
            if (!word.empty()) {
                tokens.push_back({identifyToken(word), word});
                word.clear();
            }
            tokens.push_back({TokenType::SYMBOL, string(1, ch)});
        } else {
            word += ch;
        }
    }
    if (!word.empty()) {
        tokens.push_back({identifyToken(word), word});
    }
    return tokens;
}

// Definición de clases del AST
class ASTNode {
public:
    virtual ~ASTNode() = default;
};

class FunctionDef : public ASTNode {
public:
    string name;
    vector<string> args;
    vector<shared_ptr<ASTNode>> body;
    string return_type;

    FunctionDef(const string& name, const vector<string>& args, const vector<shared_ptr<ASTNode>>& body, const string& return_type)
        : name(name), args(args), body(body), return_type(return_type) {}
};

class Assign : public ASTNode {
public:
    string target;
    shared_ptr<ASTNode> value;

    Assign(const string& target, shared_ptr<ASTNode> value) : target(target), value(value) {}
};

class Literal : public ASTNode {
public:
    string value;
    string type;

    Literal(const string& value, const string& type) : value(value), type(type) {}
};

class Return : public ASTNode {
public:
    shared_ptr<ASTNode> value;

    Return(shared_ptr<ASTNode> value) : value(value) {}
};

class If : public ASTNode {
public:
    shared_ptr<ASTNode> condition;
    vector<shared_ptr<ASTNode>> then_body;
    vector<shared_ptr<ASTNode>> else_body;

    If(shared_ptr<ASTNode> condition, const vector<shared_ptr<ASTNode>>& then_body, const vector<shared_ptr<ASTNode>>& else_body)
        : condition(condition), then_body(then_body), else_body(else_body) {}
};

class While : public ASTNode {
public:
    shared_ptr<ASTNode> condition;
    vector<shared_ptr<ASTNode>> body;

    While(shared_ptr<ASTNode> condition, const vector<shared_ptr<ASTNode>>& body)
        : condition(condition), body(body) {}
};

// Estructura para representar instrucciones de TAC
struct TAC {
    string op;
    string arg1;
    string arg2;
    string result;
};

// Función para imprimir el AST
void imprimirAST(shared_ptr<ASTNode> nodo, int nivel = 0) {
    if (auto func_def = dynamic_pointer_cast<FunctionDef>(nodo)) {
        cout << string(nivel * 2, ' ') << "FunctionDef: " << func_def->name << " -> " << func_def->return_type << endl;
        for (const auto& stmt : func_def->body) {
            imprimirAST(stmt, nivel + 1);
        }
    } else if (auto assign = dynamic_pointer_cast<Assign>(nodo)) {
        cout << string(nivel * 2, ' ') << "Assign: " << assign->target << " = ";
        imprimirAST(assign->value, 0);
    } else if (auto literal = dynamic_pointer_cast<Literal>(nodo)) {
        cout << literal->value << " (" << literal->type << ")" << endl;
    } else if (auto return_stmt = dynamic_pointer_cast<Return>(nodo)) {
        cout << string(nivel * 2, ' ') << "Return: ";
        imprimirAST(return_stmt->value, 0);
    } else if (auto if_stmt = dynamic_pointer_cast<If>(nodo)) {
        cout << string(nivel * 2, ' ') << "If: " << endl;
        cout << string((nivel + 1) * 2, ' ') << "Condition: ";
        imprimirAST(if_stmt->condition, 0);
        cout << string((nivel + 1) * 2, ' ') << "Then: " << endl;
        for (const auto& stmt : if_stmt->then_body) {
            imprimirAST(stmt, nivel + 2);
        }
        if (!if_stmt->else_body.empty()) {
            cout << string((nivel + 1) * 2, ' ') << "Else: " << endl;
            for (const auto& stmt : if_stmt->else_body) {
                imprimirAST(stmt, nivel + 2);
            }
        }
    } else if (auto while_stmt = dynamic_pointer_cast<While>(nodo)) {
        cout << string(nivel * 2, ' ') << "While: " << endl;
        cout << string((nivel + 1) * 2, ' ') << "Condition: ";
        imprimirAST(while_stmt->condition, 0);
        cout << string((nivel + 1) * 2, ' ') << "Body: " << endl;
        for (const auto& stmt : while_stmt->body) {
            imprimirAST(stmt, nivel + 2);
        }
    }
}

// Estructuras para el analizador semántico
struct Simbolo {
    string variable;
    string tipo;
    string ambito;
};

struct Funcion {
    string nombre;
    string tipo;
};

class AnalizadorSemantic {
private:
    vector<Simbolo> tabla_simbolos;
    vector<Funcion> funciones;

public:
    void analizar(shared_ptr<ASTNode> arbol) {
        visit(arbol, "global");
        imprimir_tabla_simbolos();
    }

     void visit(shared_ptr<ASTNode> nodo, const string& ambito) {
        if (auto func_def = dynamic_pointer_cast<FunctionDef>(nodo)) {
            for (const auto& arg : func_def->args) {
                agregar_variable(arg, "desconocido", func_def->name); // Tipo desconocido por ahora
            }

            for (const auto& statement : func_def->body) {
                visit(statement, func_def->name);
            }

            funciones.push_back({func_def->name, func_def->return_type});
        } else if (auto assign = dynamic_pointer_cast<Assign>(nodo)) {
            string tipo_dato = obtener_tipo_dato(assign->value);
            agregar_variable(assign->target, tipo_dato, ambito);
        } else if (auto return_stmt = dynamic_pointer_cast<Return>(nodo)) {
            // Analizar el valor de retorno
            visit(return_stmt->value, ambito);
        } else if (auto if_stmt = dynamic_pointer_cast<If>(nodo)) {
            // Analizar la condición
            visit(if_stmt->condition, ambito);
            // Analizar el cuerpo del bloque "then"
            for (const auto& stmt : if_stmt->then_body) {
                visit(stmt, ambito);
            }
            // Analizar el cuerpo del bloque "else"
            for (const auto& stmt : if_stmt->else_body) {
                visit(stmt, ambito);
            }
        } else if (auto while_stmt = dynamic_pointer_cast<While>(nodo)) {
            // Analizar la condición
            visit(while_stmt->condition, ambito);
            // Analizar el cuerpo del bucle
            for (const auto& stmt : while_stmt->body) {
                visit(stmt, ambito);
            }
        }
    }

    void agregar_variable(const string& variable, const string& tipo, const string& ambito) {
        tabla_simbolos.push_back({variable, tipo, ambito});
    }

    string obtener_tipo_dato(shared_ptr<ASTNode> nodo) {
        if (auto literal = dynamic_pointer_cast<Literal>(nodo)) {
            return literal->type;
        }
        return "desconocido";
    }

    void imprimir_tabla_simbolos() {
        cout << "------- Tabla de Símbolos ----------" << endl;
        for (const auto& simbolo : tabla_simbolos) {
            cout << "|   Variable:   | " << simbolo.variable << "   |    Tipo:   |   " << simbolo.tipo << "|     Ámbito:      |" << simbolo.ambito << endl;
        }
        cout << "------- Funciones --------" << endl;
        for (const auto& funcion : funciones) {
            cout << "|     Función:      |" << funcion.nombre << "|      Tipo de Retorno:      |" << funcion.tipo << endl;
        }
    }
};

// Generador de TAC
class TACGenerator {
private:
    vector<TAC> instrucciones;
    int temp_count = 0;
    int label_count = 0;

public:
    vector<TAC> generate(shared_ptr<ASTNode> nodo) {
        visit(nodo);
        return instrucciones;
    }

    void visit(shared_ptr<ASTNode> nodo) {
        if (auto func_def = dynamic_pointer_cast<FunctionDef>(nodo)) {
            for (const auto& stmt : func_def->body) {
                visit(stmt);
            }
        } else if (auto assign = dynamic_pointer_cast<Assign>(nodo)) {
            string result = assign->target;
            string value = obtener_valor(assign->value);
            instrucciones.push_back({"=", value, "", result});
        } else if (auto return_stmt = dynamic_pointer_cast<Return>(nodo)) {
            string value = obtener_valor(return_stmt->value);
            instrucciones.push_back({"return", value, "", ""});
        } else if (auto if_stmt = dynamic_pointer_cast<If>(nodo)) {
            string condition = obtener_valor(if_stmt->condition);
            string label_then = generar_etiqueta();
            string label_else = generar_etiqueta();
            instrucciones.push_back({"if", condition, "goto", label_then});
            for (const auto& stmt : if_stmt->else_body) {
                visit(stmt);
            }
            instrucciones.push_back({"goto", "", "", label_else});
            instrucciones.push_back({label_then + ":", "", "", ""});
            for (const auto& stmt : if_stmt->then_body) {
                visit(stmt);
            }
            instrucciones.push_back({label_else + ":", "", "", ""});
        } else if (auto while_stmt = dynamic_pointer_cast<While>(nodo)) {
            string label_start = generar_etiqueta();
            string label_end = generar_etiqueta();
            instrucciones.push_back({label_start + ":", "", "", ""});
            string condition = obtener_valor(while_stmt->condition);
            instrucciones.push_back({"if", condition, "goto", label_end});
            for (const auto& stmt : while_stmt->body) {
                visit(stmt);
            }
            instrucciones.push_back({"goto", "", "", label_start});
            instrucciones.push_back({label_end + ":", "", "", ""});
        }
    }

    string obtener_valor(shared_ptr<ASTNode> nodo) {
        if (auto literal = dynamic_pointer_cast<Literal>(nodo)) {
            return literal->value;
        }
        return "desconocido";
    }

    string generar_etiqueta() {
        return "t" + to_string(label_count++);
    }
};
// Funciones para el analizador sintáctico
bool parseExpression(const vector<Token> &tokens, size_t &pos); // Analiza y evalúa expresiones matemáticas complejas con múltiples términos y operadores.
bool parseTerm(const vector<Token> &tokens, size_t &pos); //Analiza y evalúa términos individuales en una expresión, generalmente implicando multiplicación o división.
bool parseFactor(const vector<Token> &tokens, size_t &pos); //Analiza y evalúa factores dentro de un término, que pueden ser operandos simples o subexpresiones dentro de paréntesis.
bool parseUnary(const vector<Token> &tokens, size_t &pos);// Analiza valores binarios


bool parseExpression(const vector<Token> &tokens) {
    size_t pos = 0;
    bool status = parseExpression(tokens, pos) && pos == tokens.size();
    if (!status && pos < tokens.size()) {
        cout << "Error sintáctico cerca de: " << tokens[pos].value << endl;
    }
    return status;
}

bool parseExpression(const vector<Token> &tokens, size_t &pos) {
    if (!parseTerm(tokens, pos))
        return false;
    while (pos < tokens.size() && (tokens[pos].value == "+" || tokens[pos].value == "-")) {
        pos++;
        if (!parseTerm(tokens, pos))
            return false;
    }
    return true;
}

bool parseTerm(const vector<Token> &tokens, size_t &pos) {
    if (!parseUnary(tokens, pos))
        return false;
    while (pos < tokens.size() && (tokens[pos].value == "*" || tokens[pos].value == "/")) {
        pos++;
        if (!parseUnary(tokens, pos))
            return false;
    }
    return true;
}

bool parseUnary(const vector<Token> &tokens, size_t &pos) {
    if (pos < tokens.size() && tokens[pos].value == "-") {
        pos++;
    }
    return parseFactor(tokens, pos);
}

bool parseFactor(const vector<Token> &tokens, size_t &pos) {
    if (pos < tokens.size() && tokens[pos].type == TokenType::NUMBER) {
        pos++;
        return true;
    } else if (pos < tokens.size() && tokens[pos].value == "(") {
        pos++;
        if (!parseExpression(tokens, pos))
            return false;
        if (pos < tokens.size() && tokens[pos].value == ")") {
            pos++;
            return true;
        }
        return false;
    }
    return false;
}
//Función para convertir texto a binario
string textToBinary(const string& text) {
    string binaryString;
    for (char ch : text) {
        for (int i = 7; i >= 0; --i) {
            binaryString.push_back((ch & (1 << i)) ? '1' : '0');
        }
        binaryString.push_back(' '); // Separador opcional entre bytes
    }
    return binaryString;
}
int main() {
    ifstream file("prueba.txt");

    // Verificar si el archivo se abrió correctamente
    if (!file.is_open()) {
        cerr << "No se pudo abrir el archivo." << endl;
        return 0;
    }

    // Leer el contenido del archivo
    string line;
    string content;
    vector<Token> tokens;
    while (getline(file, line)) {
        content += line + '\n';
        vector<Token> line_tokens = tokenize(line);
        tokens.insert(tokens.end(), line_tokens.begin(), line_tokens.end());
    }
    file.close();

    // Convertir el contenido a binario
    string binaryContent = textToBinary(content);

    // Escribir el contenido binario en un nuevo archivo
    ofstream outputFile("Codigo_en_Binario.bin");
    if (!outputFile.is_open()) {
        cerr << "No se pudo crear el archivo de salida." << endl;
        return 0;
    }
    outputFile << binaryContent;
    outputFile.close();

    cout << "El contenido ha sido convertido a binario y guardado en 'output.bin'." << endl;

    // Imprimir tokens
    for (const auto& token : tokens) {
        cout << "Token: " << token.value << ", Tipo: ";
        switch (token.type) {
            case TokenType::KEYWORD: cout << "Palabra clave"; break;
            case TokenType::IDENTIFIER: cout << "Identificador"; break;
            case TokenType::NUMBER: cout << "Número"; break;
            case TokenType::SYMBOL: cout << "Símbolo"; break;
            case TokenType::UNKNOWN: cout << "Desconocido"; break;
        }
        cout << endl;
    }

    // Analizar sintácticamente la secuencia de tokens
    bool isSyntacticallyCorrect = parseExpression(tokens);

    // Imprimir el resultado del análisis sintáctico
    if (isSyntacticallyCorrect) {
        cout << "La secuencia de tokens es sintácticamente correcta." << endl;
    } else {
        cout << "La secuencia de tokens no es sintácticamente correcta." << endl;
    }

    // Crear un árbol de sintaxis abstracta (AST) de ejemplo
    auto ast = make_shared<FunctionDef>("main", vector<string>{}, vector<shared_ptr<ASTNode>>{
        make_shared<Assign>("x", make_shared<Literal>("21", "int")),
        make_shared<Assign>("y", make_shared<Literal>("\"Diego\"", "string")),
        make_shared<If>(
            make_shared<Literal>("x > 10", "condition"),
            vector<shared_ptr<ASTNode>>{
                make_shared<Assign>("x", make_shared<Literal>("x + 1", "int"))
            },
            vector<shared_ptr<ASTNode>>{
                make_shared<Assign>("x", make_shared<Literal>("x - 1", "int"))
            }
        ),
        make_shared<While>(
            make_shared<Literal>("x < 30", "condition"),
            vector<shared_ptr<ASTNode>>{
                make_shared<Assign>("x", make_shared<Literal>("x + 2", "int"))
            }
        ),
        make_shared<Return>(make_shared<Literal>("x", "int"))
    }, "int"); // Agregar el tipo de retorno aquí

    // Imprimir el AST
    cout << "Árbol de Sintaxis Abstracta (AST):" << endl;
    imprimirAST(ast);

    // Realizar análisis semántico
    AnalizadorSemantic analizador;
    analizador.analizar(ast);

    // Generar código intermedio (TAC)
    TACGenerator tacGen;
    vector<TAC> tac = tacGen.generate(ast);

    // Imprimir el código intermedio (TAC)
    cout << "Código Intermedio (TAC):" << endl;
    for (const auto& instr : tac) {
        cout << instr.op << " " << instr.arg1 << " " << instr.arg2 << " " << instr.result << endl;
    }

    return 0;
}
