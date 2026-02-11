#pragma once
//**************************************
// cParamListNode.h
//
// Defines an AST node for formal parameters (args) in function declarations.
//
// Author: Phil Howard 
// phil.howard@oit.edu
//

#include "cAstNode.h"
#include "cDeclNode.h"
#include "cDeclsNode.h"

class cParamListNode : public cDeclsNode
{
    public:
        // param is the first param in this list
        cParamListNode(cDeclNode *param) : cDeclsNode(param)
        {
        }

        virtual string NodeType() { return string("args"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};
