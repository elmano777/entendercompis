#ifndef VISITOR_H
#define VISITOR_H
#include "ast.h"
#include <list>
#include <unordered_map>

class BinaryExp;
class NumberExp;
class SqrtExp;

class Visitor {
public:
    virtual int visit(BinaryExp* exp) = 0;
    virtual int visit(NumberExp* exp) = 0;
    virtual int visit(IdExp* exp) = 0;
    virtual int visit(SqrtExp* exp) = 0;
    virtual void visit(AsignStmt* stm) = 0;
    virtual void visit(PrintStmt* stm) = 0;
    virtual void visit(Programa* program) = 0;
    virtual void visit(IfStmt* stm) = 0;
    virtual int visit(boolExp* exp) = 0;
    virtual int visit(compExp* exp) = 0;
    virtual int visit(CallExp* exp) = 0;
    virtual void visit(WhileStmt* stm) = 0;
    virtual void visit(ReturnStmt* stm) = 0;
    virtual void visit(VarDec* stm) = 0;
    virtual void visit(FunDec* stm) = 0;
};

class PrintVisitor : public Visitor {
public:
    int visit(boolExp* exp) override;
    int visit(compExp* exp) override;
    int visit(CallExp* exp) override;
    void visit(WhileStmt* stm) override;
    void visit(ReturnStmt* stm) override;
    void visit(VarDec* stm) override;
    void visit(FunDec* stm) override;
    int visit(BinaryExp* exp) override;
    int visit(NumberExp* exp) override;
    int visit(SqrtExp* exp) override;
    void visit(AsignStmt* stm) override;
    void visit(PrintStmt* stm) override;
    void visit(Programa* program) override;
    int visit(IdExp* exp) override;
    void visit(IfStmt* stm) override;
    void imprimir(Programa* program);
};

class EVALVisitor : public Visitor {
public:
    unordered_map<string,int> memoria;
    // Agregar en EVALVisitor:
    unordered_map<string, FunDec*> funciones;
    int returnValue = 0;
    bool hasReturn = false;
    int visit(BinaryExp* exp) override;
    int visit(NumberExp* exp) override;
    int visit(SqrtExp* exp) override;
    void visit(AsignStmt* stm) override;
    void visit(PrintStmt* stm) override;
    int visit(IdExp* exp) override;
    void visit(Programa* program) override;
    void visit(IfStmt* stm) override;
    void interprete(Programa* program);
    int visit(boolExp* exp) override;
    int visit(compExp* exp) override;
    int visit(CallExp* exp) override;
    void visit(WhileStmt* stm) override;
    void visit(ReturnStmt* stm) override;
    void visit(VarDec* stm) override;
    void visit(FunDec* stm) override;
};


#endif // VISITOR_H
