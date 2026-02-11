#pragma once
//**************************************
// cFuncDeclNode.h
//
// Defines an AST node for function declarations.
//
// Author: Phil Howard 
// phil.howard@oit.edu
//

#include "cAstNode.h"
#include "cDeclNode.h"
#include "cDeclsNode.h"
#include "cStmtsNode.h"
#include "cSymbol.h"

class cFuncDeclNode : public cDeclNode
{
    public:
        // constructor for function declaration/prototype without params
        cFuncDeclNode(cSymbol *returnType, cSymbol *name) : cDeclNode()
        {
            AddChild(returnType);
            AddChild(name);
        }

        // constructor for function with params but no body
        cFuncDeclNode(cSymbol *returnType, cSymbol *name, cDeclsNode *params) : cDeclNode()
        {
            AddChild(returnType);
            AddChild(name);
            AddChild(params);
        }

        // constructor for function with params and body
        cFuncDeclNode(cSymbol *returnType, cSymbol *name, cDeclsNode *params, cStmtsNode *stmts) : cDeclNode()
        {
            AddChild(returnType);
            AddChild(name);
            AddChild(params);
            AddChild(stmts);
        }

        // Public method to add children (for params or stmts)
        void AddChild(cAstNode *child)
        {
            cAstNode::AddChild(child);
        }

        virtual string NodeType() { return string("func"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};
