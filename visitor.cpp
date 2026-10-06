#include <iostream>
#include <cmath>
#include "ast.h"
#include "visitor.h"


using namespace std;

///////////////////////////////////////////////////////////////////////////////////
int BinaryExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int NumberExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int IdExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int SqrtExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

void PrintStmt::accept(Visitor* visitor) {
    visitor->visit(this);
}

void AsignStmt::accept(Visitor* visitor) {
    visitor->visit(this);
}

void Programa::accept(Visitor* visitor) {
    visitor->visit(this);
}

void IfStmt::accept(Visitor* visitor) {
    visitor->visit(this);
}

int boolExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int compExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int CallExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

void WhileStmt::accept(Visitor* visitor) {
    visitor->visit(this);
}

void ReturnStmt::accept(Visitor* visitor) {
    visitor->visit(this);
}

void VarDec::accept(Visitor* visitor) {
    visitor->visit(this);
}

void FunDec::accept(Visitor* visitor) {
    visitor->visit(this);
}

///////////////////////////////////////////////////////////////////////////////////

int PrintVisitor::visit(BinaryExp* exp) {
    exp->left->accept(this);
    cout << ' ' << Exp::binopToChar(exp->op) << ' ';
    exp->right->accept(this);
    return 0;
}

int PrintVisitor::visit(NumberExp* exp) {
    cout << exp->value;
    return 0;
}

int PrintVisitor::visit(SqrtExp* exp) {
    cout << "sqrt(";
    exp->value->accept(this);
    cout <<  ")";
    return 0;
}


void PrintVisitor::imprimir(Programa* programa){
    if (programa)
    {
        cout << "Codigo:" << endl;
        programa->accept(this);
        cout << endl;
    }
    return ;
}

int PrintVisitor::visit(boolExp* exp) {
    cout << (exp->value ? "true" : "false");
    return 0;
}

int PrintVisitor::visit(compExp* exp) {
    exp->left->accept(this);
    if (exp->op == LT_OP) cout << " < ";
    else if (exp->op == LE_OP) cout << " <= ";
    else if (exp->op == EQ_OP) cout << " == ";
    exp->right->accept(this);
    return 0;
}

int PrintVisitor::visit(CallExp* exp) {
    cout << exp->name << "(";
    bool first = true;
    for (auto arg : exp->args) {
        if (!first) cout << ", ";
        arg->accept(this);
        first = false;
    }
    cout << ")";
    return 0;
}

void PrintVisitor::visit(WhileStmt* stm) {
    cout << "while (";
    stm->cond->accept(this);
    cout << ") do" << endl;
    for (auto s : stm->body) {
        s->accept(this);
    }
    cout << "endwhile" << endl;
}

void PrintVisitor::visit(ReturnStmt* stm) {
    cout << "return";
    if (stm->exp) {
        cout << "(";
        stm->exp->accept(this);
        cout << ")";
    }
    cout << endl;
}

void PrintVisitor::visit(VarDec* stm) {
    cout << "var " << stm->type << " ";
    bool first = true;
    for (auto v : stm->vars) {
        if (!first) cout << ", ";
        cout << v;
        first = false;
    }
    cout << ";" << endl;
}

void PrintVisitor::visit(FunDec* stm) {
    cout << "fun " << stm->type << " " << stm->name << "(";
    bool first = true;
    for (auto p : stm->params) {
        if (!first) cout << ", ";
        cout << p.first << " " << p.second;
        first = false;
    }
    cout << ")" << endl;
    for (auto s : stm->body) {
        s->accept(this);
    }
    cout << "endfun" << endl;
}

///////////////////////////////////////////////////////////////////////////////////
int EVALVisitor::visit(BinaryExp* exp) {
    int result;
    int v1 = exp->left->accept(this);
    int v2 = exp->right->accept(this);
    switch (exp->op) {
        case PLUS_OP:
            result = v1 + v2;
            break;
        case MINUS_OP:
            result = v1 - v2;
            break;
        case MUL_OP:
            result = v1 * v2;
            break;
        case DIV_OP:
            if (v2 != 0)
                result = v1 / v2;
            else {
                cout << "Error: división por cero" << endl;
                result = 0;
            }
            break;
        case POW_OP:
            result = pow(v1,v2);
            break;
        default:
            cout << "Operador desconocido" << endl;
            result = 0;
    }
    return result;
}

int EVALVisitor::visit(NumberExp* exp) {
    return exp->value;
}

int EVALVisitor::visit(SqrtExp* exp) {
    return floor(sqrt( exp->value->accept(this)));
}


void EVALVisitor::interprete(Programa* programa) {
    if (programa) {
        cout << "Interprete:" << endl;
        programa->accept(this);
        // llamar a main si existe
        if (funciones.count("main")) {
            list<Exp*> args;
            CallExp call("main", args);
            call.accept(this);
        }
        cout << endl;
    }
}

void EVALVisitor::visit(AsignStmt *stm) {
    memoria[stm->variable]=stm->exp->accept(this);
}

int EVALVisitor::visit(IdExp *e) {
    return memoria[e->value];
}

int EVALVisitor::visit(boolExp* exp) {
    return exp->value ? 1 : 0;
}

int EVALVisitor::visit(compExp* exp) {
    int l = exp->left->accept(this);
    int r = exp->right->accept(this);
    if (exp->op == LT_OP) return l < r;
    else if (exp->op == LE_OP) return l <= r;
    else if (exp->op == EQ_OP) return l == r;
    return 0;
}

void EVALVisitor::visit(WhileStmt* stm) {
    while (stm->cond->accept(this)) {
        for (auto s : stm->body) {
            s->accept(this);
        }
    }
}

void EVALVisitor::visit(ReturnStmt* stm) {
    if (stm->exp) {
        returnValue = stm->exp->accept(this);
    }
    hasReturn = true;
}

void EVALVisitor::visit(VarDec* stm) {
    for (auto v : stm->vars) {
        memoria[v] = 0;  // inicializa en 0
    }
}

void EVALVisitor::visit(FunDec* stm) {
    funciones[stm->name] = stm;  // guarda la función en memoria
}

int EVALVisitor::visit(CallExp* exp) {
    FunDec* fun = funciones[exp->name];
    // guardar argumentos en memoria
    auto it = fun->params.begin();
    for (auto arg : exp->args) {
        memoria[it->second] = arg->accept(this);
        ++it;
    }
    // ejecutar cuerpo
    hasReturn = false;
    returnValue = 0;
    for (auto s : fun->body) {
        s->accept(this);
        if (hasReturn) break;
    }
    return returnValue;
}


void EVALVisitor::visit(PrintStmt *stm) {
    cout << stm->exp->accept(this) << endl;
}

void EVALVisitor::visit(Programa *p) {
    for (auto i : p->varDeclList) {
        i->accept(this);
    }
    for (auto i : p->funDeclList) {
        i->accept(this);
    }
    for (auto i : p->slist) {
        i->accept(this);
    }
}



void PrintVisitor::visit(AsignStmt *stm) {
    cout << stm->variable << " = ";
    stm->exp->accept(this);
    cout << endl;
}
void PrintVisitor::visit(PrintStmt *stm) {
    cout << "print (";
    stm->exp->accept(this);
    cout << ")"<< endl;
}

void PrintVisitor::visit(Programa* p) {
    for (auto i : p->varDeclList) {
        i->accept(this);
    }
    for (auto i : p->funDeclList) {
        i->accept(this);
    }
    for (auto i : p->slist) {
        i->accept(this);
    }
}

int PrintVisitor::visit(IdExp *e) {
    cout << e->value;
    return 0;
}

void PrintVisitor::visit(IfStmt *stm) {
    cout << "if (";
    stm->cond->accept(this);
    cout << ") then" << endl;
    for (auto i : stm->thenList) {
        i->accept(this);
    }
    cout << "else" << endl;
    for (auto i : stm->elseList) {
        i->accept(this);
    }
    cout << "endif" << endl;  // ← faltaba esto
}

void EVALVisitor::visit(IfStmt *stm) {
    if (stm->cond->accept(this)) {
        for (auto i:stm->thenList) {
            i->accept(this);
        }
    } else {
        for (auto i:stm->elseList) {
            i->accept(this);
        }
    }
}
