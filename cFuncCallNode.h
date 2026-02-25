#pragma once
//**************************************
// cFuncCallNode.h
//
// Defines an AST node for function calls.
//
// Author: Phil Howard 
// phil.howard@oit.edu
//

#include "cAstNode.h"
#include "cExprNode.h"
#include "cSymbol.h"
#include "cParamsNode.h"

class cFuncDeclNode;  // Forward declaration

class cFuncCallNode : public cExprNode
{
    public:
        // constructor for function call without params
        cFuncCallNode(cSymbol *funcName) : cExprNode()
        {
            AddChild(funcName);
        }

        // constructor for function call with params
        cFuncCallNode(cSymbol *funcName, cAstNode *params) : cExprNode()
        {
            AddChild(funcName);
            AddChild(params);
        }

        virtual string NodeType() { return string("funcCall"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
        
        // GetDecl returns the function declaration
        virtual cDeclNode *GetDecl()
        {
            cSymbol *funcSym = dynamic_cast<cSymbol*>(GetChild(0));
            if (funcSym != nullptr)
                return funcSym->GetDecl();
            return nullptr;
        }
        
        // GetType returns the return type of the function
        virtual cDeclNode *GetType()
        {
            cSymbol *funcSym = dynamic_cast<cSymbol*>(GetChild(0));
            if (funcSym != nullptr && funcSym->GetDecl() != nullptr)
                return funcSym->GetDecl()->GetType();
            return nullptr;
        }
        
        // Public accessors for semantic checking
        cSymbol* GetFuncSymbol() 
        { 
            return dynamic_cast<cSymbol*>(GetChild(0));
        }
        
        cParamsNode* GetParams() 
        { 
            if (NumChildren() >= 2)
                return dynamic_cast<cParamsNode*>(GetChild(1));
            return nullptr;
        }
        
        int GetNumArgs()
        {
            cParamsNode *params = GetParams();
            return params ? params->GetNumParams() : 0;
        }
};
