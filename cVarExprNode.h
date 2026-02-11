#pragma once
//**************************************
// cVarExprNode.h
//
// Defines an AST node for a variable reference in an expression.
//
// Author: Phil Howard 
// phil.howard@oit.edu
//

#include "cAstNode.h"
#include "cExprNode.h"
#include "cSymbol.h"

class cVarExprNode : public cExprNode
{
    public:
        // constructor taking a symbol
        cVarExprNode(cSymbol *symbol) : cExprNode()
        {
            AddChild(symbol);
        }

        // Method to add additional symbols (for member access like a.b)
        void AddSymbol(cSymbol *symbol)
        {
            AddChild(symbol);
        }

        // Method to add any child (for array indices, etc.)
        void AddChild(cAstNode *child)
        {
            cAstNode::AddChild(child);
        }

        virtual string NodeType() { return string("varref"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};
