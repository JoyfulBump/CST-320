//**************************************************************
// cCodeGen.cpp
//
// Code generation visitor for stackl assembly output
//
// Author: Phil Howard
// Email: phil.howard@oit.edu
//
//**************************************************************

#include "cCodeGen.h"
#include "emit.h"
#include "astnodes.h"
#include <sstream>

using std::string;
using std::stringstream;

cCodeGen::cCodeGen(string filename) : cVisitor()
{
    m_filename = filename;
    InitOutput(filename);
}

cCodeGen::~cCodeGen()
{
    FinalizeOutput();
}

void cCodeGen::Visit(cProgramNode *node)
{
    EmitString(".function main\nmain:\n");
    if (node != nullptr)
    {
        cBlockNode *block = node->GetBlock();
        if (block != nullptr) block->Visit(this);
    }
    EmitString("    PUSH 0\n");
    EmitString("    RETURNV\n");
}

void cCodeGen::Visit(cBlockNode *node)
{
    if (node == nullptr) return;
    int blockSize = node->GetSize();
    if (blockSize > 0)
    {
        stringstream ss;
        ss << "    ADJSP " << blockSize << "\n";
        EmitString(ss.str());
    }
    cDeclsNode *decls = node->GetDecls();
    if (decls != nullptr) decls->Visit(this);
    cStmtsNode *stmts = node->GetStmts();
    if (stmts != nullptr) stmts->Visit(this);
}

void cCodeGen::Visit(cDeclsNode *node)
{
    node->VisitAllChildren(this);
}

void cCodeGen::Visit(cVarDeclNode *node)
{
    // Variable declarations don't generate code
}

void cCodeGen::Visit(cStmtsNode *node)
{
    node->VisitAllChildren(this);
}

void cCodeGen::Visit(cPrintNode *node)
{
    if (node == nullptr) return;
    cExprNode *expr = node->GetExpr();
    if (expr != nullptr)
    {
        expr->Visit(this);
    }
    EmitString("    CALL @print\n");
    EmitString("    POP\n");
}

void cCodeGen::Visit(cIntExprNode *node)
{
    if (node != nullptr)
    {
        stringstream ss;
        ss << "    PUSH " << node->GetValue() << "\n";
        EmitString(ss.str());
    }
}

void cCodeGen::Visit(cVarExprNode *node)
{
    if (node == nullptr) return;
    int offset = node->GetOffset();
    stringstream ss;
    ss << "    PUSHVAR " << offset << "\n";
    EmitString(ss.str());
}

void cCodeGen::Visit(cAssignNode *node)
{
    if (node == nullptr) return;
    cExprNode *rval = node->GetRVal();
    cAstNode *lval = node->GetLVal();
    
    // Evaluate right-hand side
    if (rval != nullptr)
    {
        rval->Visit(this);
    }
    
    // Duplicate the value
    EmitString("    DUP\n");
    
    // Calculate address and store
    cVarExprNode *varRef = dynamic_cast<cVarExprNode*>(lval);
    if (varRef != nullptr)
    {
        int offset = varRef->GetOffset();
        stringstream ss;
        ss << "    PUSH " << offset << "\n";
        EmitString(ss.str());
        EmitString("    PUSHFP\n");
        EmitString("    PLUS\n");
        EmitString("    POPVARIND\n");
    }
    
    // Clean up
    EmitString("    POP\n");
}

void cCodeGen::Visit(cBinaryExprNode *node)
{
    if (node == nullptr) return;
    cExprNode *left = node->GetLeft();
    cExprNode *right = node->GetRight();
    cOpNode *op = node->GetOp();
    
    if (left != nullptr) left->Visit(this);
    if (right != nullptr) right->Visit(this);
    
    if (op != nullptr)
    {
        int opVal = op->GetOp();
        switch(opVal)
        {
            case '+':
                EmitString("    PLUS\n");
                break;
            case '-':
                EmitString("    MINUS\n");
                break;
            case '*':
                EmitString("    TIMES\n");
                break;
            case '/':
                EmitString("    DIVIDE\n");
                break;
            case '%':
                EmitString("    MOD\n");
                break;
            case '<':
                EmitString("    LT\n");
                break;
            case '>':
                EmitString("    GT\n");
                break;
            case 267:  // EQUALS
                EmitString("    EQ\n");
                break;
            case 268:  // NOT_EQUALS
                EmitString("    NE\n");
                break;
            case 263:  // LE
                EmitString("    LE\n");
                break;
            case 264:  // GE
                EmitString("    GE\n");
                break;
            case 265:  // AND
                EmitString("    AND\n");
                break;
            case 266:  // OR
                EmitString("    OR\n");
                break;
        }
    }
}

void cCodeGen::Visit(cWhileNode *node)
{
    if (node == nullptr) return;
    string startLabel = GenerateLabel();
    string endLabel = GenerateLabel();
    EmitString(startLabel + ":\n");
    cExprNode *cond = node->GetCondition();
    if (cond != nullptr) cond->Visit(this);
    EmitString("    JUMPE @" + endLabel + "\n");
    cStmtNode *stmt = node->GetStatement();
    if (stmt != nullptr) stmt->Visit(this);
    EmitString("    JUMP @" + startLabel + "\n");
    EmitString(endLabel + ":\n");
}

void cCodeGen::Visit(cIfNode *node)
{
    if (node == nullptr) return;
    string elseLabel = GenerateLabel();
    string endLabel = GenerateLabel();
    cExprNode *cond = node->GetCondition();
    if (cond != nullptr) cond->Visit(this);
    EmitString("    JUMPE @" + elseLabel + "\n");
    cStmtsNode *trueStmts = node->GetTrueStmts();
    if (trueStmts != nullptr) trueStmts->Visit(this);
    cStmtsNode *falseStmts = node->GetFalseStmts();
    if (falseStmts != nullptr)
    {
        EmitString("    JUMP @" + endLabel + "\n");
        EmitString(elseLabel + ":\n");
        falseStmts->Visit(this);
        EmitString(endLabel + ":\n");
    }
    else
    {
        EmitString(elseLabel + ":\n");
    }
}

void cCodeGen::Visit(cReturnNode *node)
{
    if (node != nullptr)
    {
        cExprNode *expr = node->GetExpr();
        if (expr != nullptr)
        {
            expr->Visit(this);
        }
    }
    EmitString("    RETURNV\n");
}

void cCodeGen::Visit(cFuncCallNode *node)
{
    if (node != nullptr) node->VisitAllChildren(this);
}

// Stub implementations
void cCodeGen::Visit(cArrayDeclNode *node) { if(node) node->VisitAllChildren(this); }
void cCodeGen::Visit(cAstNode *node) { if(node) node->VisitAllChildren(this); }
void cCodeGen::Visit(cBaseTypeNode *node) { if(node) node->VisitAllChildren(this); }
void cCodeGen::Visit(cDeclNode *node) { if(node) node->VisitAllChildren(this); }
void cCodeGen::Visit(cExprNode *node) { if(node) node->VisitAllChildren(this); }
void cCodeGen::Visit(cFloatExprNode *node) { if(node) node->VisitAllChildren(this); }
void cCodeGen::Visit(cFuncDeclNode *node) { if(node) node->VisitAllChildren(this); }
void cCodeGen::Visit(cOpNode *node) { }
void cCodeGen::Visit(cParamListNode *node) { if(node) node->VisitAllChildren(this); }
void cCodeGen::Visit(cParamsNode *node) { if(node) node->VisitAllChildren(this); }
void cCodeGen::Visit(cPrintsNode *node) { if(node) node->VisitAllChildren(this); }
void cCodeGen::Visit(cStmtNode *node) { if(node) node->VisitAllChildren(this); }
void cCodeGen::Visit(cStructDeclNode *node) { if(node) node->VisitAllChildren(this); }
void cCodeGen::Visit(cSymbol *node) { }
