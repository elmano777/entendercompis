#include "ast.h"

using namespace std;

// ------------------ Exp ------------------
Exp::~Exp() {}

string Exp::binopToChar(BinaryOp op) {
  switch (op) {
  case PLUS_OP:
    return "+";
  case MINUS_OP:
    return "-";
  case MUL_OP:
    return "*";
  case DIV_OP:
    return "/";
  case POW_OP:
    return "**";
  default:
    return "?";
  }
}

// ------------------ BinaryExp ------------------
BinaryExp::BinaryExp(Exp *l, Exp *r, BinaryOp o) : left(l), right(r), op(o) {}

BinaryExp::~BinaryExp() {
  delete left;
  delete right;
}

// ------------------ NumberExp ------------------
NumberExp::NumberExp(int v) : value(v) {}

NumberExp::~NumberExp() {}

// ------------------ SqrtExp ------------------
SqrtExp::SqrtExp(Exp *v) : value(v) {}

SqrtExp::~SqrtExp() {}

//
Programa::Programa() {}
Programa::~Programa() {}

Stmt::~Stmt() {}

PrintStmt::PrintStmt(Exp *e) { exp = e; }

PrintStmt::~PrintStmt() {}

IfStmt::IfStmt(Exp *c) { cond = c; }

IfStmt::~IfStmt() { delete cond; }

AsignStmt::AsignStmt(string texto, Exp *e) {
  variable = texto;
  exp = e;
}

AsignStmt::~AsignStmt() {}

// ------------------ Exp ------------------
IdExp::IdExp(string v) : value(v) {}

IdExp::~IdExp() {}

boolExp::boolExp(bool v) : value(v) {}

boolExp::~boolExp() {}

compExp::compExp(Exp *l, Exp *r, BinaryOp o) : left(l), right(r), op(o) {}

compExp::~compExp() {
  delete left;
  delete right;
}

CallExp::CallExp(string n, list<Exp *> a) : name(n), args(a) {}

CallExp::~CallExp() {
  for (auto arg : args)
    delete arg;
}

// ------------------ stmt ------------------
WhileStmt::WhileStmt(Exp *c, list<Stmt *> b) : cond(c), body(b) {}

WhileStmt::~WhileStmt() {
  delete cond;
  for (auto s : body)
    delete s;
}

ReturnStmt::ReturnStmt(Exp *e) : exp(e) {}

ReturnStmt::~ReturnStmt() {
  if (exp)
    delete exp;
}

