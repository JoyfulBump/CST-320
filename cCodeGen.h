#pragma once
//**************************************************************
// cCodeGen.h
//
// Code generation visitor for stackl assembly output
//
// Author: Phil Howard
// Email: phil.howard@oit.edu
//
//**************************************************************

#include "cVisitor.h"
#include <string>

using std::string;

class cCodeGen : public cVisitor
{
public:
    cCodeGen(string filename);
    virtual ~cCodeGen();

    virtual void Visit(cArrayDeclNode *node);
    virtual void Visit(cAssignNode *node);
    virtual void Visit(cAstNode *node);
    virtual void Visit(cBaseTypeNode *node);
    virtual void Visit(cBinaryExprNode *node);
    virtual void Visit(cBlockNode *node);
    virtual void Visit(cDeclNode *node);
    virtual void Visit(cDeclsNode *node);
    virtual void Visit(cExprNode *node);
    virtual void Visit(cFloatExprNode *node);
    virtual void Visit(cFuncCallNode *node);
    virtual void Visit(cFuncDeclNode *node);
    virtual void Visit(cIfNode *node);
    virtual void Visit(cIntExprNode *node);
    virtual void Visit(cOpNode *node);
    virtual void Visit(cParamListNode *node);
    virtual void Visit(cParamsNode *node);
    virtual void Visit(cPrintNode *node);
    virtual void Visit(cPrintsNode *node);
    virtual void Visit(cProgramNode *node);
    virtual void Visit(cReturnNode *node);
    virtual void Visit(cStmtNode *node);
    virtual void Visit(cStmtsNode *node);
    virtual void Visit(cStructDeclNode *node);
    virtual void Visit(cSymbol *node);
    virtual void Visit(cVarDeclNode *node);
    virtual void Visit(cVarExprNode *node);
    virtual void Visit(cWhileNode *node);

private:
    string m_filename;
};
