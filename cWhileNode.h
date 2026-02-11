#pragma once
//**************************************
// cWhileNode.h
//
// Defines an AST node for while statements.
//
// Author: Phil Howard 
// phil.howard@oit.edu
//

#include "cAstNode.h"
#include "cStmtNode.h"
#include "cExprNode.h"

class cWhileNode : public cStmtNode
{
    public:
        // constructor taking condition and statement
        cWhileNode(cExprNode *cond, cStmtNode *stmt) : cStmtNode()
        {
            AddChild(cond);
            AddChild(stmt);
        }

        virtual string NodeType() { return string("while"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};
