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

        cExprNode* GetCondition()
        {
            return static_cast<cExprNode*>(GetChild(0));
        }

        cStmtNode* GetStatement()
        {
            return static_cast<cStmtNode*>(GetChild(1));
        }

        virtual string NodeType() { return string("while"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};
