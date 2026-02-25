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
#include "cSymbolTable.h"
#include "cSymbol.h"

extern cSymbolTable g_symbolTable;

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

        virtual cDeclNode *GetType()
        {
            // Get operands and operator
            cExprNode *left = dynamic_cast<cExprNode*>(GetChild(0));
            cOpNode *op = dynamic_cast<cOpNode*>(GetChild(1));
            cExprNode *right = dynamic_cast<cExprNode*>(GetChild(2));
            
            if (left == nullptr || right == nullptr || op == nullptr)
                return nullptr;
            
            int opType = op->GetOp();
            
            // Relational operators (< > <= >= == !=) return int
            if (opType == '<' || opType == '>' || 
                opType == 263 || opType == 264 ||  // LE, GE
                opType == 267 || opType == 268)     // EQUALS, NOT_EQUALS
            {
                cSymbol *intSym = g_symbolTable.Find("int");
                if (intSym != nullptr && intSym->GetDecl() != nullptr)
                    return intSym->GetDecl()->GetType();
                return nullptr;
            }
            
            // Logical operators (&& ||) return int
            if (opType == 265 || opType == 266)  // AND, OR
            {
                cSymbol *intSym = g_symbolTable.Find("int");
                if (intSym != nullptr && intSym->GetDecl() != nullptr)
                    return intSym->GetDecl()->GetType();
                return nullptr;
            }
            
            // For arithmetic operators, promote types
            cDeclNode *leftType = left->GetType();
            cDeclNode *rightType = right->GetType();
            
            if (leftType == nullptr) return rightType;
            if (rightType == nullptr) return leftType;
            
            // If either is float, result is float
            if (leftType->IsFloat() || rightType->IsFloat())
            {
                // Return the larger float type
                if (leftType->IsFloat() && rightType->IsFloat())
                {
                    // Return larger size
                    return (leftType->GetSize() >= rightType->GetSize()) ? leftType : rightType;
                }
                return leftType->IsFloat() ? leftType : rightType;
            }
            
            // Both are integer types - return larger size
            return (leftType->GetSize() >= rightType->GetSize()) ? leftType : rightType;
        }

        virtual string NodeType() { return string("expr"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};
