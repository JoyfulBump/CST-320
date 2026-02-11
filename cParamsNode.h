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
        }

        // Add a param to the list
        void Insert(cExprNode *param)
        {
            AddChild(param);
        }

        virtual string NodeType() { return string("params"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};
