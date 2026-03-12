#pragma once
//**************************************
// cProgramNode.h
//
// Defines AST node for a lang program
//
// Author: Phil Howard 
// phil.howard@oit.edu
//

#include "cAstNode.h"
#include "cBlockNode.h"

class cProgramNode : public cAstNode
{
    public:
        // param is the block that makes up the program
        cProgramNode(cBlockNode *block) : cAstNode()
        {
            AddChild(block);
            m_size = 0;
        }

        virtual string NodeType() { return string("program"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
        
        cBlockNode* GetBlock()
        {
            if (HasChildren())
                return static_cast<cBlockNode*>(GetChild(0));
            return nullptr;
        }
        
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
