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
            m_size = size;
        }

        virtual string AttributesToString() 
        {
            return " count=\"" + std::to_string(m_size) + "\"";
        }

        virtual string NodeType() { return string("array_decl"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
    
    protected:
        int m_size;
};
