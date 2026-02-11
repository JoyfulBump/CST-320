#pragma once
//**************************************
// cIfNode.h
//
// Defines an AST node for if statements.
//
// Author: Phil Howard 
// phil.howard@oit.edu
//

#include "cAstNode.h"
#include "cStmtNode.h"
#include "cExprNode.h"
#include "cStmtsNode.h"

class cIfNode : public cStmtNode
{
    public:
        // constructor for if without else
        cIfNode(cExprNode *cond, cStmtsNode *trueStmts) : cStmtNode()
        {
            AddChild(cond);
            AddChild(trueStmts);
        }

        // constructor for if with else
        cIfNode(cExprNode *cond, cStmtsNode *trueStmts, cStmtsNode *falseStmts) : cStmtNode()
        {
            AddChild(cond);
            AddChild(trueStmts);
            AddChild(falseStmts);
        }

        virtual string NodeType() { return string("if"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};
