#pragma once
//**************************************
// cArrayDeclNode.h
//
// Defines an AST node for array declarations.
//
// Author: Phil Howard 
// phil.howard@oit.edu
//

#include "cAstNode.h"
#include "cDeclNode.h"
#include "cSymbol.h"

class cArrayDeclNode : public cDeclNode
{
    public:
        // constructor taking type, size, and array name
        cArrayDeclNode(cSymbol *type, int size, cSymbol *name) : cDeclNode()
        {
            AddChild(type);
            AddChild(name);
            m_count = size;  // Store the array dimension separately
        }

        virtual string AttributesToString() 
        {
            string result = "";
            if (m_size > 0 || m_offset > 0)
            {
                result += " count=\"" + std::to_string(m_count) + "\"";
                result += " size=\"" + std::to_string(m_size) + "\"";
                result += " offset=\"" + std::to_string(m_offset) + "\"";
            }
            return result;
        }
        
        // Getter for array dimension
        int GetCount() { return m_count; }

        virtual bool IsArray() { return true; }
        virtual bool IsType() { return true; }
        virtual cDeclNode *GetType() 
        { 
            cSymbol *typeSym = dynamic_cast<cSymbol*>(GetChild(0));
            if (typeSym != nullptr && typeSym->GetDecl() != nullptr)
                return typeSym->GetDecl()->GetType();
            return nullptr;
        }

        cDeclNode *GetBaseDecl()
        {
            cSymbol *typeSym = dynamic_cast<cSymbol*>(GetChild(0));
            if (typeSym != nullptr)
                return typeSym->GetDecl();
            return nullptr;
        }
        virtual int GetSize()
        {
            cSymbol *typeSym = dynamic_cast<cSymbol*>(GetChild(0));
            if (typeSym != nullptr && typeSym->GetDecl() != nullptr)
                return m_count * typeSym->GetDecl()->GetSize();
            return 0;
        }
        
        virtual string GetName() 
        { 
            cSymbol *sym = dynamic_cast<cSymbol*>(GetChild(1));
            return sym ? sym->GetName() : "";
        }

        virtual string NodeType() { return string("array_decl"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
    
    protected:
        int m_count;  // Number of elements in the array
};
