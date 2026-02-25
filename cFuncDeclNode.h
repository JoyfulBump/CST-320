#pragma once
//**************************************
// cFuncDeclNode.h
//
// Defines an AST node for function declarations.
//
// Author: Phil Howard 
// phil.howard@oit.edu
//

#include "cAstNode.h"
#include "cDeclNode.h"
#include "cDeclsNode.h"
#include "cStmtsNode.h"
#include "cSymbol.h"
#include "cParamListNode.h"

class cFuncDeclNode : public cDeclNode
{
    public:
        // constructor for function declaration/prototype without params
        cFuncDeclNode(cSymbol *returnType, cSymbol *name) : cDeclNode()
        {
            AddChild(returnType);
            AddChild(name);
        }

        // constructor for function with params but no body
        cFuncDeclNode(cSymbol *returnType, cSymbol *name, cDeclsNode *params) : cDeclNode()
        {
            AddChild(returnType);
            AddChild(name);
            AddChild(params);
        }

        // constructor for function with params and body
        cFuncDeclNode(cSymbol *returnType, cSymbol *name, cDeclsNode *params, cStmtsNode *stmts) : cDeclNode()
        {
            AddChild(returnType);
            AddChild(name);
            AddChild(params);
            AddChild(stmts);
        }

        // Public method to add children (for params or stmts)
        void AddChild(cAstNode *child)
        {
            cAstNode::AddChild(child);
        }

        virtual bool IsFunc() { return true; }
        virtual cDeclNode *GetType() 
        { 
            cSymbol *typeSym = dynamic_cast<cSymbol*>(GetChild(0));
            if (typeSym != nullptr && typeSym->GetDecl() != nullptr)
                return typeSym->GetDecl()->GetType();
            return nullptr;
        }
        
        virtual string GetName() 
        { 
            cSymbol *sym = GetFuncSymbol();
            return sym ? sym->GetName() : "";
        }
        
        // Check if function has a body (definition vs declaration)
        bool HasBody()
        {
            // If there are more than 3 children, we have decls and/or stmts
            // Child 0: return type, Child 1: name, Child 2: params (optional), Child 3+: body
            // Body is present if last child is a cStmtsNode
            if (NumChildren() >= 3)
            {
                cAstNode *lastChild = GetChild(NumChildren() - 1);
                return dynamic_cast<cStmtsNode*>(lastChild) != nullptr;
            }
            return false;
        }
        
        // Get number of parameters
        int GetNumParams()
        {
            // Child 2 (if exists) is the parameter list
            if (NumChildren() >= 3)
            {
                cParamListNode *params = dynamic_cast<cParamListNode*>(GetChild(2));
                if (params != nullptr)
                {
                    return params->GetNumDecls();
                }
            }
            return 0; // No parameters
        }
        
        // Get the parameter list
        cParamListNode* GetParamList()
        {
            if (NumChildren() >= 3)
            {
                return dynamic_cast<cParamListNode*>(GetChild(2));
            }
            return nullptr;
        }
        
        // Get the function's symbol (name)
        cSymbol* GetFuncSymbol()
        {
            // Child 1 is always the name symbol
            if (NumChildren() >= 2)
            {
                return dynamic_cast<cSymbol*>(GetChild(1));
            }
            return nullptr;
        }
        
        // Copy body from another function (for forward declarations after definitions)
        void CopyBodyFrom(cFuncDeclNode* other)
        {
            if (other != nullptr && other->HasBody())
            {
                // Find where the body starts in the other function
                // Children: type (0), name (1), [params (2)], [decls], stmts
                // Body elements are cDeclsNode or cStmtsNode
                // We copy everything that is NOT a symbol or param list
                for (int i = 0; i < other->NumChildren(); i++)
                {
                    cAstNode* child = other->GetChild(i);
                    // Skip symbols (type and name) and params
                    if (dynamic_cast<cSymbol*>(child) == nullptr && 
                        dynamic_cast<cParamListNode*>(child) == nullptr)
                    {
                        cAstNode::AddChild(child);
                    }
                }
            }
        }

        virtual string NodeType() { return string("func"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};
