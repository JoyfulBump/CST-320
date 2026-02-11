#pragma once
//**************************************
// cBinaryExprNode.h
//
// Defines an AST node for binary expressions.
//
// Inherits from cExprNode
//
// Author: Phil Howard 
// phil.howard@oit.edu
//

#include "cAstNode.h"
#include "cExprNode.h"
#include "cOpNode.h"

class cBinaryExprNode : public cExprNode
{
    public:
        // constructor taking left operand, operator, and right operand
        cBinaryExprNode(cExprNode *left, cOpNode *op, cExprNode *right) 
            : cExprNode()
        {
            AddChild(left);
            AddChild(op);
            AddChild(right);
        }

        virtual string NodeType() { return string("expr"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};
