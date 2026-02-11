#pragma once
//**************************************
// cFuncCallNode.h
//
// Defines an AST node for function calls.
//
// Author: Phil Howard 
// phil.howard@oit.edu
//

#include "cAstNode.h"
#include "cExprNode.h"
#include "cSymbol.h"

class cFuncCallNode : public cExprNode
{
    public:
        // constructor for function call without params
        cFuncCallNode(cSymbol *funcName) : cExprNode()
        {
            AddChild(funcName);
        }

        // constructor for function call with params
        cFuncCallNode(cSymbol *funcName, cAstNode *params) : cExprNode()
        {
            AddChild(funcName);
            AddChild(params);
        }

        virtual string NodeType() { return string("funcCall"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};
