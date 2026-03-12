#pragma once
//**************************************
// cDeclsNode.h
//
// Defines a class to represent a list of declarations.
//
// Author: Phil Howard 
// phil.howard@oit.edu
//

#include "cAstNode.h"
#include "cDeclNode.h"

class cDeclsNode : public cAstNode
{
    public:
        // param is the first decl in this decls
        cDeclsNode(cDeclNode *decl) : cAstNode()
        {
            AddChild(decl);
            m_size = 0;
        }

        // Add a decl to the list
        void Insert(cDeclNode *decl)
        {
            AddChild(decl);
        }
        
        // Public accessors for children (needed for size calculations)
        int GetNumDecls() { return NumChildren(); }
        cDeclNode* GetDecl(int index) { return dynamic_cast<cDeclNode*>(GetChild(index)); }

        virtual string NodeType() { return string("decls"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
        
        // Size getter/setter
        void SetSize(int size) { m_size = size; }
        int GetSize() { return m_size; }
        
        virtual string AttributesToString()
        {
            string result = "";
            if (m_size > 0)
            {
                result += " size=\"" + std::to_string(m_size) + "\"";
            }
            return result;
        }
        
    protected:
        int m_size;
};
