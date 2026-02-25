#pragma once
//**************************************
// cVarDeclNode.h
//
// Defines an AST node for a variable declaration.
//
// Author: Phil Howard 
// phil.howard@oit.edu
//

#include "cAstNode.h"
#include "cDeclNode.h"
#include "cSymbol.h"

class cVarDeclNode : public cDeclNode
{
    public:
        // constructor taking type and variable name
        cVarDeclNode(cSymbol *type, cSymbol *name) : cDeclNode()
        {
            AddChild(type);
            AddChild(name);
        }

        virtual bool IsVar() { return true; }
        virtual cDeclNode *GetType() 
        { 
            cSymbol *typeSym = dynamic_cast<cSymbol*>(GetChild(0));
            if (typeSym != nullptr && typeSym->GetDecl() != nullptr)
                return typeSym->GetDecl()->GetType();
            return nullptr;
        }
        
        // Get the immediate type declaration (not ultimate type)
        cDeclNode* GetTypeDecl()
        {
            cSymbol *typeSym = dynamic_cast<cSymbol*>(GetChild(0));
            if (typeSym != nullptr)
                return typeSym->GetDecl();
            return nullptr;
        }
        
        virtual string GetName() 
        { 
            cSymbol *sym = dynamic_cast<cSymbol*>(GetChild(1));
            return sym ? sym->GetName() : "";
        }

        virtual string NodeType() { return string("var_decl"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};
