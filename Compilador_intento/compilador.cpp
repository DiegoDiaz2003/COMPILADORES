#include <iostream>
#include <fstream>
#include <cctype>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
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
    if (word.size() == 1 && string("+-*/%(){}=;<>").find(word) != string::npos) {
        return TokenType::SYMBOL;
    }

    // Verificar si es una cadena (simplificación: solo cadenas entre comillas dobles)
    if (word.size() >= 2 && word.front() == '"' && word.back() == '"') {
        return TokenType::IDENTIFIER; // Puedes definir un nuevo tipo de token para cadenas si lo prefieres
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
        } else if (string("+-*/%(){}=;<>").find(ch) != string::npos) {
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

// imprimir
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



// Otros nodos del AST como Return, AnnAssign, etc.

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

        // Otros tipos de nodos...

        // Recorrer hijos del nodo (si los tiene)
    }

    string obtener_tipo_dato(shared_ptr<ASTNode> nodo) {
        if (auto literal = dynamic_pointer_cast<Literal>(nodo)) {
            return literal->type;
        }
        // Otros casos para diferentes tipos de nodos...
        return "desconocido";
    }

    void agregar_variable(const string& nombre, const string& tipo, const string& ambito_actual) {
        // Verificar si la variable ya existe en el ámbito actual
        auto it = find_if(tabla_simbolos.begin(), tabla_simbolos.end(),
                          [&nombre, &ambito_actual](const Simbolo& simbolo) {
                              return simbolo.variable == nombre && simbolo.ambito == ambito_actual;
                          });

        if (it == tabla_simbolos.end()) {
            // Si no existe, agregar la nueva variable
            tabla_simbolos.push_back({nombre, tipo, ambito_actual});
        } else {
            // Si ya existe, actualizar el tipo si es necesario
            it->tipo = tipo;
        }
    }

    void imprimir_tabla_simbolos() {
        cout << "Variable\tTipo\tÁmbito\n";
        for (const auto& simbolo : tabla_simbolos) {
            cout << simbolo.variable << "\t" << simbolo.tipo << "\t" << simbolo.ambito << "\n";
        }
    }
};



// Funciones para el analizador sintáctico
bool parseExpression(const vector<Token> &tokens, size_t &pos); // Analiza y evalúa expresiones matemáticas complejas con múltiples términos y operadores.
bool parseTerm(const vector<Token> &tokens, size_t &pos); //Analiza y evalúa términos individuales en una expresión, generalmente implicando multiplicación o división.
bool parseFactor(const vector<Token> &tokens, size_t &pos); //Analiza y evalúa factores dentro de un término, que pueden ser operandos simples o subexpresiones dentro de paréntesis.
bool parseUnary(const vector<Token> &tokens, size_t &pos);// Analiza valores binarios

class BinaryOp : public ASTNode {
public:
    string op;
    shared_ptr<ASTNode> left;
    shared_ptr<ASTNode> right;

    BinaryOp(const string& op, shared_ptr<ASTNode> left, shared_ptr<ASTNode> right)
        : op(op), left(left), right(right) {}
};

class UnaryOp : public ASTNode {
public:
    string op;
    shared_ptr<ASTNode> operand;

    UnaryOp(const string& op, shared_ptr<ASTNode> operand)
        : op(op), operand(operand) {}
};



class Parser {
public:
    Parser(const vector<Token>& tokens) : tokens(tokens), pos(0) {}

    shared_ptr<ASTNode> parse() {
        return parseFunction();
    }

private:
    vector<Token> tokens;
    size_t pos;

    shared_ptr<ASTNode> parseFunction() {
        if (tokens[pos].type == TokenType::KEYWORD && tokens[pos].value == "int") {
            pos++;
            if (tokens[pos].type == TokenType::IDENTIFIER) {
                string funcName = tokens[pos].value;
                pos++;
                if (tokens[pos].type == TokenType::SYMBOL && tokens[pos].value == "(") {
                    pos++;
                    // Ignorar argumentos por simplicidad
                    if (tokens[pos].type == TokenType::SYMBOL && tokens[pos].value == ")") {
                        pos++;
                        if (tokens[pos].type == TokenType::SYMBOL && tokens[pos].value == "{") {
                            pos++;
                            vector<shared_ptr<ASTNode>> body;
                            while (pos < tokens.size() && (tokens[pos].type != TokenType::SYMBOL || tokens[pos].value != "}")) {
                                auto stmt = parseStatement();
                                if (stmt) {
                                    body.push_back(stmt);
                                } else {
                                    cerr << "Error al analizar la declaración en la función." << endl;
                                    return nullptr;
                                }
                            }
                            if (pos < tokens.size() && tokens[pos].type == TokenType::SYMBOL && tokens[pos].value == "}") {
                                pos++;
                                return make_shared<FunctionDef>(funcName, vector<string>{}, body, "int");
                            }
                        }
                    }
                }
            }
        }
        return nullptr;
    }

    shared_ptr<ASTNode> parseStatement() {
        if (tokens[pos].type == TokenType::KEYWORD && (tokens[pos].value == "int" || tokens[pos].value == "string")) {
            return parseDeclaration();
        } else if (tokens[pos].type == TokenType::KEYWORD && tokens[pos].value == "if") {
            return parseIf();
        } else if (tokens[pos].type == TokenType::KEYWORD && tokens[pos].value == "while") {
            return parseWhile();
        } else if (tokens[pos].type == TokenType::KEYWORD && tokens[pos].value == "return") {
            return parseReturn();
        } else {
            return parseAssignment();
        }
    }

    shared_ptr<ASTNode> parseDeclaration() {
        string type = tokens[pos].value;
        pos++;
        if (tokens[pos].type == TokenType::IDENTIFIER) {
            string varName = tokens[pos].value;
            pos++;
            if (tokens[pos].type == TokenType::SYMBOL && tokens[pos].value == "=") {
                pos++;
                auto value = parseExpression();
                if (tokens[pos].type == TokenType::SYMBOL && tokens[pos].value == ";") {
                    pos++;
                    return make_shared<Assign>(varName, value);
                }
            }
        }
        return nullptr;
    }

    shared_ptr<ASTNode> parseIf() {
        pos++;
        if (tokens[pos].type == TokenType::SYMBOL && tokens[pos].value == "(") {
            pos++;
            auto condition = parseExpression();
            if (tokens[pos].type == TokenType::SYMBOL && tokens[pos].value == ")") {
                pos++;
                if (tokens[pos].type == TokenType::SYMBOL && tokens[pos].value == "{") {
                    pos++;
                    vector<shared_ptr<ASTNode>> thenBody;
                    while (pos < tokens.size() && (tokens[pos].type != TokenType::SYMBOL || tokens[pos].value != "}")) {
                        thenBody.push_back(parseStatement());
                    }
                    pos++;
                    vector<shared_ptr<ASTNode>> elseBody;
                    if (tokens[pos].type == TokenType::KEYWORD && tokens[pos].value == "else") {
                        pos++;
                        if (tokens[pos].type == TokenType::SYMBOL && tokens[pos].value == "{") {
                            pos++;
                            while (pos < tokens.size() && (tokens[pos].type != TokenType::SYMBOL || tokens[pos].value != "}")) {
                                elseBody.push_back(parseStatement());
                            }
                            pos++;
                        }
                    }
                    return make_shared<If>(condition, thenBody, elseBody);
                }
            }
        }
        return nullptr;
    }

    shared_ptr<ASTNode> parseWhile() {
        pos++;
        if (tokens[pos].type == TokenType::SYMBOL && tokens[pos].value == "(") {
            pos++;
            auto condition = parseExpression();
            if (tokens[pos].type == TokenType::SYMBOL && tokens[pos].value == ")") {
                pos++;
                if (tokens[pos].type == TokenType::SYMBOL && tokens[pos].value == "{") {
                    pos++;
                    vector<shared_ptr<ASTNode>> body;
                    while (pos < tokens.size() && (tokens[pos].type != TokenType::SYMBOL || tokens[pos].value != "}")) {
                        body.push_back(parseStatement());
                    }
                    pos++;
                    return make_shared<While>(condition, body);
                }
            }
        }
        return nullptr;
    }

    shared_ptr<ASTNode> parseReturn() {
        pos++;
        auto value = parseExpression();
        if (tokens[pos].type == TokenType::SYMBOL && tokens[pos].value == ";") {
            pos++;
            return make_shared<Return>(value);
        }
        return nullptr;
    }

    shared_ptr<ASTNode> parseAssignment() {
        if (tokens[pos].type == TokenType::IDENTIFIER) {
            string varName = tokens[pos].value;
            pos++;
            if (tokens[pos].type == TokenType::SYMBOL && tokens[pos].value == "=") {
                pos++;
                auto value = parseExpression();
                if (tokens[pos].type == TokenType::SYMBOL && tokens[pos].value == ";") {
                    pos++;
                    return make_shared<Assign>(varName, value);
                }
            }
        }
        return nullptr;
    }

    shared_ptr<ASTNode> parseExpression() {
        return parseBinaryOp(0);
    }

    shared_ptr<ASTNode> parseBinaryOp(int minPrecedence) {
        auto left = parseUnary();
        while (pos < tokens.size() && getPrecedence(tokens[pos].value) >= minPrecedence) {
            string op = tokens[pos].value;
            int precedence = getPrecedence(op);
            pos++;
            auto right = parseUnary();
            while (pos < tokens.size() && getPrecedence(tokens[pos].value) > precedence) {
                right = parseBinaryOp(precedence + 1);
            }
            left = make_shared<BinaryOp>(op, left, right);
        }
        return left;
    }

    shared_ptr<ASTNode> parseUnary() {
        if (tokens[pos].value == "-") {
            pos++;
            auto node = parsePrimary();
            return make_shared<UnaryOp>("-", node);
        }
        return parsePrimary();
    }

    shared_ptr<ASTNode> parsePrimary() {
        if (tokens[pos].type == TokenType::NUMBER) {
            string value = tokens[pos].value;
            pos++;
            return make_shared<Literal>(value, "int");
        } else if (tokens[pos].type == TokenType::IDENTIFIER) {
            string value = tokens[pos].value;
            pos++;
            return make_shared<Literal>(value, "string");
        } else if (tokens[pos].type == TokenType::SYMBOL && tokens[pos].value == "(") {
            pos++;
            auto node = parseExpression();
            if (tokens[pos].type == TokenType::SYMBOL && tokens[pos].value == ")") {
                pos++;
                return node;
            }
        }
        return nullptr;
    }

    int getPrecedence(const string &op) {
        if (op == "+" || op == "-") return 1;
        if (op == "*" || op == "/") return 2;
        return 0;
    }
};

int main() {
    ifstream file("prueba.txt");

    // Verificar si el archivo se abrió correctamente
    if (!file.is_open()) {
        cout << "Error al abrir el archivo." << endl;
        return 1; // O gestiona el error de otra manera
    }

    string line;
    vector<Token> tokens;

    // Leer cada línea del archivo
    while (getline(file, line)) {
        vector<Token> line_tokens = tokenize(line);
        tokens.insert(tokens.end(), line_tokens.begin(), line_tokens.end());
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

    // Crear el parser y analizar los tokens
    Parser parser(tokens);
    auto ast = parser.parse();

    // Verificar si el análisis sintáctico fue exitoso
    if (ast) {
        cout << "La secuencia de tokens es sintácticamente correcta." << endl;
        cout << "Árbol de Sintaxis Abstracta (AST):" << endl;
        imprimirAST(ast);
    } else {
        cout << "Error al analizar la declaración en la función." << endl;
        return 1;
    }

    // Analizar semánticamente el AST y imprimir la tabla de símbolos
    AnalizadorSemantic analizador;
    analizador.analizar(ast);

    return 0;
}

