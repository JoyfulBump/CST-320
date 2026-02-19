#pragma once
//**************************************
// cIntExprNode.h
//
// Defines an AST node for an integer constant (literals).
//
// Inherits from cExprNode so that integer constants can be used anywhere 
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

class cIntExprNode : public cExprNode
{
    public:
        // param is the value of the integer constant
        cIntExprNode(int value) : cExprNode()
        {
            m_value = value;
            
        }

        virtual string AttributesToString() 
        {
            return " value=\"" + std::to_string(m_value) + "\"";
        }
        
        virtual cDeclNode *GetType()
        {
            // If value is in char range (0-127), return char type
            // Otherwise return int type
            if (m_value >= 0 && m_value <= 127)
            {
                cSymbol *charSym = g_symbolTable.Find("char");
                if (charSym != nullptr && charSym->GetDecl() != nullptr)
                    return charSym->GetDecl()->GetType();
            }
            
            cSymbol *intSym = g_symbolTable.Find("int");
            if (intSym != nullptr && intSym->GetDecl() != nullptr)
                return intSym->GetDecl()->GetType();
            
            return nullptr;
        }
        
        virtual string NodeType() { return string("int"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
    protected:
        int m_value;        // value of integer constant (literal)
};
