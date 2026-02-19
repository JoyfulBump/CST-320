#pragma once
//**************************************
// cFloatExprNode.h
//
// Defines an AST node for a float constant (literals).
//
// Inherits from cExprNode so that float constants can be used anywhere 
// expressions are used.
//
// Author: Phil Howard 
// phil.howard@oit.edu
//

#include "cAstNode.h"
#include "cExprNode.h"
#include "cSymbolTable.h"
#include "cSymbol.h"

extern cSymbolTable g_symbolTable;

class cFloatExprNode : public cExprNode
{
    public:
        // param is the value of the float constant
        cFloatExprNode(float value) : cExprNode()
        {
            m_value = value;
        }

        virtual string AttributesToString() 
        {
            return " value=\"" + std::to_string(m_value) + "\"";
        }
        
        virtual cDeclNode *GetType()
        {
            cSymbol *floatSym = g_symbolTable.Find("float");
            if (floatSym != nullptr && floatSym->GetDecl() != nullptr)
                return floatSym->GetDecl()->GetType();
            return nullptr;
        }
        
        virtual string NodeType() { return string("float"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
    protected:
        float m_value;        // value of float constant (literal)
};
