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
        
        // Public accessors for semantic checking
        int GetNumChildren() { return NumChildren(); }
        cAstNode* GetChildAt(int index) { return GetChild(index); }

        virtual string NodeType() { return string("varref"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
        
        // GetDecl returns the declaration of the variable
        virtual cDeclNode *GetDecl()
        {
            // For simple variable: return decl from symbol
            cSymbol *sym = dynamic_cast<cSymbol*>(GetChild(0));
            if (sym != nullptr)
                return sym->GetDecl();
            return nullptr;
        }
        
        // GetType returns the type of the variable reference
        virtual cDeclNode *GetType()
        {
            // For simple variable: return its type
            cSymbol *sym = dynamic_cast<cSymbol*>(GetChild(0));
            if (sym != nullptr && sym->GetDecl() != nullptr)
                return sym->GetDecl()->GetType();
            return nullptr;
        }
};
