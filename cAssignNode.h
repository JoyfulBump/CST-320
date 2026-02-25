#pragma once
//**************************************
// cAssignNode.h
//
// Defines an AST node for assignment statements.
//
// Author: Phil Howard 
// phil.howard@oit.edu
//

#include "cAstNode.h"
#include "cStmtNode.h"
#include "cExprNode.h"

class cAssignNode : public cStmtNode
{
    public:
        // constructor taking lval and expression
        cAssignNode(cAstNode *lval, cExprNode *expr) : cStmtNode()
        {
            AddChild(lval);
            AddChild(expr);
        }

        cAstNode* GetLVal() { return GetChild(0); }
        cExprNode* GetRVal() { return dynamic_cast<cExprNode*>(GetChild(1)); }

        virtual string NodeType() { return string("assign"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};
