#include "parser.h"
#include "ast.h"
#include "scanner.h"
#include "token.h"
#include <iostream>
#include <stdexcept>

using namespace std;

// =============================
// Métodos de la clase Parser
// =============================

Parser::Parser(Scanner *sc) : scanner(sc) {
  previous = nullptr;
  current = scanner->nextToken();
  if (current->type == Token::ERR) {
    throw runtime_error("Error léxico");
  }
}

bool Parser::match(Token::Type ttype) {
  if (check(ttype)) {
    advance();
    return true;
  }
  return false;
}

bool Parser::check(Token::Type ttype) {
  if (isAtEnd())
    return false;
  return current->type == ttype;
}

bool Parser::advance() {
  if (!isAtEnd()) {
    Token *temp = current;
    if (previous)
      delete previous;
    current = scanner->nextToken();
    previous = temp;

    if (check(Token::ERR)) {
      throw runtime_error("Error lexico");
    }
    return true;
  }
  return false;
}

bool Parser::isAtEnd() { return (current->type == Token::END); }

// =============================
// Reglas gramaticales
// =============================

Programa *Parser::parseProgram() {
  Programa *p = new Programa();

  // VarDecList ::= (VarDec)*
  while (check(Token::VAR)) {
    p->varDeclList.push_back(parseVarDec());
  }

  // FunDecList ::= (FunDec)+
  if (!check(Token::FUN)) {
    throw runtime_error("Se esperaba al menos una función");
  }
  while (check(Token::FUN)) {
    p->funDeclList.push_back(parseFunDec());
  }

  if (!isAtEnd()) {
    throw runtime_error("Error sintáctico");
  }
  cout << "Parseo exitoso" << endl;
  return p;
}

Stmt *Parser::parseVarDec() {
  match(Token::VAR);
  string type = current->text;
  match(Token::ID); // el tipo: int, bool, etc
  list<string> vars;
  vars.push_back(current->text);
  match(Token::ID); // primera variable
  while (match(Token::COMMA)) {
    vars.push_back(current->text);
    match(Token::ID);
  }
  match(Token::SEMICOL);
  return new VarDec(type, vars);
}

Stmt *Parser::parseFunDec() {
  match(Token::FUN);
  string type = current->text;
  match(Token::ID); // tipo de retorno
  string name = current->text;
  match(Token::ID); // nombre función
  match(Token::LPAREN);
  list<pair<string, string>> params;
  if (!check(Token::RPAREN)) {
    params = parseParamDecList();
  }
  match(Token::RPAREN);
  list<Stmt *> body = parseBody();
  match(Token::ENDF);
  return new FunDec(type, name, params, body);
}

list<pair<string, string>> Parser::parseParamDecList() {
  list<pair<string, string>> params;
  string type = current->text;
  match(Token::ID);
  string name = current->text;
  match(Token::ID);
  params.push_back({type, name});
  while (match(Token::COMMA)) {
    type = current->text;
    match(Token::ID);
    name = current->text;
    match(Token::ID);
    params.push_back({type, name});
  }
  return params;
}

list<Stmt *> Parser::parseBody() {
  list<Stmt *> body;
  while (check(Token::VAR)) {
    body.push_back(parseVarDec());
  }
  while (!check(Token::ENDF) && !check(Token::ENDIF) && !check(Token::ELSE) &&
         !check(Token::ENDWHILE) && !isAtEnd()) {
    body.push_back(parsestmt());
    if (check(Token::ENDF) || check(Token::ENDIF) || check(Token::ELSE) ||
        check(Token::ENDWHILE) || isAtEnd())
      break;
    match(Token::SEMICOL);
  }
  return body;
}

list<Stmt *> Parser::parseStmtList() {
  list<Stmt *> stmts;
  stmts.push_back(parsestmt());
  while (match(Token::SEMICOL)) {
    if (check(Token::ENDF) || check(Token::ENDIF) || check(Token::ELSE) ||
        check(Token::ENDWHILE) || check(Token::RETURN) || isAtEnd()) {
      break;
    }
    stmts.push_back(parsestmt());
  }
  return stmts;
}

Stmt *Parser::parsestmt() {
  cerr << "parsestmt: current = " << current << endl;
  Exp *e;
  if (match(Token::PRINT)) {
    match(Token::LPAREN);
    e = parseCEXP();
    match(Token::RPAREN);
    return new PrintStmt(e);
  } else if (match(Token::IF)) {
    e = parseCEXP();
    match(Token::THEN);
    list<Stmt *> thenList;
    while (!check(Token::ELSE) && !check(Token::ENDIF)) {
      thenList.push_back(parsestmt());
      if (check(Token::ELSE) || check(Token::ENDIF))
        break;
      match(Token::SEMICOL);
    }
    match(Token::ELSE);
    list<Stmt *> elseList;
    while (!check(Token::ENDIF)) {
      elseList.push_back(parsestmt());
      if (check(Token::ENDIF))
        break;
      match(Token::SEMICOL);
    }
    match(Token::ENDIF);
    return new IfStmt(e, thenList, elseList);
  } else if (match(Token::ID)) {
    string texto = previous->text;
    match(Token::ASSIGN);
    e = parseCEXP();
    return new AsignStmt(texto, e);
  } else if (match(Token::WHILE)) {
    Exp *cond = parseCEXP();
    match(Token::DO);
    list<Stmt *> body = parseBody();
    match(Token::ENDWHILE);
    return new WhileStmt(cond, body);
  } else if (match(Token::RETURN)) {
    if (check(Token::SEMICOL) || isAtEnd()) {
      return new ReturnStmt(nullptr);
    }
    return new ReturnStmt(parseCEXP());
  } else {
    throw runtime_error("Error sintáctico en statement");
  }
}

Exp *Parser::parseCEXP() {
  Exp *l = parseE();
  if (match(Token::LESSTHAN) || match(Token::LESSEQ) || match(Token::EQ)) {
    BinaryOp op;
    if (previous->type == Token::LESSTHAN)
      op = LT_OP;
    else if (previous->type == Token::LESSEQ)
      op = LE_OP;
    else
      op = EQ_OP;
    Exp *r = parseE();
    return new compExp(l, r, op);
  }
  return l;
}

Exp *Parser::parseE() {
  Exp *l = parseT();
  while (match(Token::PLUS) || match(Token::MINUS)) {
    BinaryOp op;
    if (previous->type == Token::PLUS) op = PLUS_OP;
    else op = MINUS_OP;
    Exp *r = parseT();
    l = new BinaryExp(l, r, op);
  }
  return l;
}

Exp *Parser::parseT() {
  Exp *l = parseF();
  while (match(Token::MUL) || match(Token::DIV)) {
    BinaryOp op;
    if (previous->type == Token::MUL) op = MUL_OP;
    else op = DIV_OP;
    Exp *r = parseF();
    l = new BinaryExp(l, r, op);
  }
  return l;
}

Exp *Parser::parseF() {
  Exp *e;
  if (match(Token::NUM)) {
    return new NumberExp(stoi(previous->text));
  } else if (match(Token::ID)) {
    string name = previous->text;
    if (match(Token::LPAREN)) { // es una llamada foo(...)
      list<Exp *> args;
      if (!check(Token::RPAREN)) {
        args.push_back(parseCEXP());
        while (match(Token::COMMA)) {
          args.push_back(parseCEXP());
        }
      }
      match(Token::RPAREN);
      return new CallExp(name, args);
    }
    return new IdExp(name); // es una variable
  } else if (match(Token::LPAREN)) {
    e = parseCEXP();
    match(Token::RPAREN);
    return e;
  } else if (match(Token::SQRT)) {
    match(Token::LPAREN);
    e = parseCEXP();
    match(Token::RPAREN);
    return new SqrtExp(e);
  } else if (match(Token::TRUE)) {
    return new boolExp(true);
  } else if (match(Token::FALSE)) {
    return new boolExp(false);
  } else {
    throw runtime_error("Error sintáctico");
  }
}
