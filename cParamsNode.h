#pragma once
//**************************************
// cParamsNode.h
//
// Defines an AST node for a list of parameters in function calls.
//
// Author: Phil Howard 
// phil.howard@oit.edu
//

#include "cAstNode.h"
#include "cExprNode.h"

class cParamsNode : public cAstNode
{
    public:
        // param is the first param in this list
        cParamsNode(cExprNode *param) : cAstNode()
        {
            AddChild(param);
            m_size = 0;
        }

        // Add a param to the list
        void Insert(cExprNode *param)
        {
            AddChild(param);
        }
        
        // Public accessors for semantic checking
        int GetNumParams() { return NumChildren(); }
        cExprNode* GetParam(int index) 
        { 
            return dynamic_cast<cExprNode*>(GetChild(index));
        }

        virtual string NodeType() { return string("params"); }
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
