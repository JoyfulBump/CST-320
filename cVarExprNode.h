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
            m_size = 0;
            m_offset = 0;
            m_rowsizes = "";
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
            cDeclNode *resultType = nullptr;
            cSymbol *lastSym = nullptr;

            for (int i = 0; i < NumChildren(); i++)
            {
                cSymbol *sym = dynamic_cast<cSymbol*>(GetChild(i));
                if (sym != nullptr)
                {
                    lastSym = sym;
                    if (sym->GetDecl() != nullptr)
                    {
                        resultType = sym->GetDecl()->GetType();
                    }
                }
            }

            // If the last symbol in the chain has no decl (unresolved reference),
            // return nullptr to signal a type error
            if (lastSym != nullptr && lastSym->GetDecl() == nullptr)
            {
                return nullptr;
            }

            return resultType;
        }
        
        // Size, offset, and rowsizes getters/setters
        void SetSize(int size) { m_size = size; }
        int GetSize() { return m_size; }
        void SetOffset(int offset) { m_offset = offset; }
        int GetOffset() { return m_offset; }
        void SetRowsizes(string rowsizes) { m_rowsizes = rowsizes; }
        string GetRowsizes() { return m_rowsizes; }
        
        virtual string AttributesToString()
        {
            string result = "";
            if (m_size > 0 || m_offset > 0)
            {
                result += " size=\"" + std::to_string(m_size) + "\"";
                result += " offset=\"" + std::to_string(m_offset) + "\"";
                if (!m_rowsizes.empty())
                {
                    result += " rowsizes=\"" + m_rowsizes + "\"";
                }
            }
            return result;
        }
        
    protected:
        int m_size;
        int m_offset;
        string m_rowsizes;
};
