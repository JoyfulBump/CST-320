#pragma once
//**************************************
// cStructDeclNode.h
//
// Defines an AST node for struct declarations.
//
// Author: Phil Howard 
// phil.howard@oit.edu
//

#include "cAstNode.h"
#include "cDeclNode.h"
#include "cDeclsNode.h"
#include "cSymbol.h"

class cStructDeclNode : public cDeclNode
{
    public:
        // constructor taking declarations and struct name
        cStructDeclNode(cDeclsNode *decls, cSymbol *name) : cDeclNode()
        {
            AddChild(decls);
            AddChild(name);
        }

        virtual bool IsStruct() { return true; }
        virtual bool IsType() { return true; }
        virtual cDeclNode *GetType() { return this; }
        virtual int GetSize()
        {
            // Sum of all member sizes
            int total = 0;
            cDeclsNode *decls = dynamic_cast<cDeclsNode*>(GetChild(0));
            if (decls != nullptr)
            {
                for (int i = 0; i < decls->GetNumDecls(); i++)
                {
                    cDeclNode *decl = decls->GetDecl(i);
                    if (decl != nullptr)
                    {
                        total += decl->GetSize();
                    }
                }
            }
            return total;
        }

        virtual string NodeType() { return string("struct_decl"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
};
