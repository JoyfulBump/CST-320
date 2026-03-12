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
        // GetSize just returns the stored value - computation done in visitor
        virtual int GetSize() { return m_size; }
        
        virtual string GetName() 
        { 
            cSymbol *sym = dynamic_cast<cSymbol*>(GetChild(1));
            return sym ? sym->GetName() : "";
        }

        virtual string NodeType() { return string("struct_decl"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
        
        cDeclsNode* GetDecls()
        {
            if (HasChildren())
                return dynamic_cast<cDeclsNode*>(GetChild(0));
            return nullptr;
        }
        
        virtual string AttributesToString()
        {
            string result = "";
            if (m_size > 0 || m_offset > 0)
            {
                result += " size=\"" + std::to_string(m_size) + "\"";
                result += " offset=\"" + std::to_string(m_offset) + "\"";
            }
            return result;
        }
};
