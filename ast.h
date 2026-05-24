#ifndef AST_H
#define AST_H

#include <list>
#include <ostream>
#include <string>
#include <unordered_map>

using namespace std;

class Visitor;

// Operadores binarios soportados
enum BinaryOp {
  PLUS_OP,
  MINUS_OP,
  MUL_OP,
  DIV_OP,
  POW_OP,
  LT_OP,
  LE_OP,
  EQ_OP, // <, <= y ==
};

// Clase abstracta Exp
class Exp {
public:
  virtual int accept(Visitor *visitor) = 0;
  virtual ~Exp() = 0;                     // Destructor puro → clase abstracta
  static string binopToChar(BinaryOp op); // Conversión operador → string
};

// Expresión binaria
class BinaryExp : public Exp {
public:
  Exp *left;
  Exp *right;
  BinaryOp op;
  int accept(Visitor *visitor);
  BinaryExp(Exp *l, Exp *r, BinaryOp op);
  ~BinaryExp();
};

// Expresión numérica
class NumberExp : public Exp {
public:
  int value;
  int accept(Visitor *visitor);
  NumberExp(int v);
  ~NumberExp();
};

class IdExp : public Exp {
public:
  string value;
  int accept(Visitor *visitor);
  IdExp(string v);
  ~IdExp();
};

// Raiz cuadrada
class SqrtExp : public Exp {
public:
  Exp *value;
  int accept(Visitor *visitor);
  SqrtExp(Exp *v);
  ~SqrtExp();
};

// bool
class boolExp : public Exp {
public:
  bool value; // true o false
  int accept(Visitor *visitor);
  boolExp(bool v);
  ~boolExp();
};

class compExp : public Exp {
public:
  Exp *left;
  Exp *right;
  BinaryOp op; // LT_OP, LE_OP, EQ_OP
  int accept(Visitor *visitor);
  compExp(Exp *l, Exp *r, BinaryOp o);
  ~compExp();
};

class CallExp : public Exp {
public:
  string name;      // nombre de la función: "foo"
  list<Exp *> args; // argumentos: 1, 2, 3
  int accept(Visitor *visitor);
  CallExp(string n, list<Exp *> a);
  ~CallExp();
};

class Stmt {
public:
  virtual void accept(Visitor *visitor) = 0;
  virtual ~Stmt() = 0;
};

class IfStmt : public Stmt {
public:
  Exp *cond;
  list<Stmt *> thenList;
  list<Stmt *> elseList;
  void accept(Visitor *visitor) override;
  IfStmt(Exp *c);
  ~IfStmt();
};

class AsignStmt : public Stmt {
public:
  string variable;
  Exp *exp;
  void accept(Visitor *visitor) override;
  AsignStmt(string, Exp *);
  ~AsignStmt();
};

class PrintStmt : public Stmt {
public:
  Exp *exp;
  void accept(Visitor *visitor) override;
  PrintStmt(Exp *e);
  ~PrintStmt();
};

class WhileStmt : public Stmt {
public:
  Exp *cond;         // La condición: x < 5
  list<Stmt *> body; // El cuerpo del while
  void accept(Visitor *visitor) override;
  WhileStmt(Exp *c, list<Stmt *> b);
  ~WhileStmt();
};

class ReturnStmt : public Stmt {
public:
  Exp *exp; // El valor a retornar (puede ser nullptr si no hay)
  void accept(Visitor *visitor) override;
  ReturnStmt(Exp *e);
  ~ReturnStmt();
};

class Programa {
public:
  list<Stmt *> varDeclList; // declaraciones de variables
  list<Stmt *> funDeclList; // declaraciones de funciones
  list<Stmt *> slist;       // sentencias (el main, si quieres)
  void accept(Visitor *visitor);
  ~Programa();
  Programa();
};

class VarDec : public Stmt {
public:
  string type;       // "int", "bool", etc.
  list<string> vars; // lista de variables: [x, y, z]
  void accept(Visitor *visitor) override;
  VarDec(string t, list<string> v);
  ~VarDec();
};

class FunDec : public Stmt {
public:
  string type;                       // tipo de retorno: "int"
  string name;                       // nombre de función: "foo"
  list<pair<string, string>> params; // [("int", "x"), ("bool", "y")]
  list<Stmt *> body;                 // cuerpo de la función
  void accept(Visitor *visitor) override;
  FunDec(string t, string n, list<pair<string, string>> p, list<Stmt *> b);
  ~FunDec();
};

#endif // AST_H
