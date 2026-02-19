#pragma once
//********************************************************
// cSymbol.h - Define a class for symbols
//
// Author: Philip Howard
//
#include <string>

using std::string;
#include "cAstNode.h"
#include "cDeclNode.h"

class cSymbol : public cAstNode
{
    public:
        // Construct a symbol given its name
        cSymbol(string name)
        {
            m_id = ++nextId;
            m_name = name;
            m_decl = nullptr;
        }

        // Return name of symbol
        string GetName() { return m_name; }
        // Return ID of symbol
        long long GetId() { return m_id; }
        
        // Decl management
        void SetDecl(cDeclNode *decl) { m_decl = decl; }
        cDeclNode *GetDecl() { return m_decl; }
        
        // Type checking - can be overridden by subclasses
        virtual bool IsArray()  { return m_decl != nullptr && m_decl->IsArray(); }
        virtual bool IsStruct() { return m_decl != nullptr && m_decl->IsStruct(); }
        virtual bool IsFunc()   { return m_decl != nullptr && m_decl->IsFunc(); }
        virtual bool IsVar()    { return m_decl != nullptr && m_decl->IsVar(); }
        virtual bool IsFloat()  { return m_decl != nullptr && m_decl->IsFloat(); }
        virtual bool IsInt()    { return m_decl != nullptr && m_decl->IsInt(); }
        virtual bool IsChar()   { return m_decl != nullptr && m_decl->IsChar(); }
        virtual int  GetSize()  { return m_decl != nullptr ? m_decl->GetSize() : 0; }

        virtual string AttributesToString() 
        {
            string result(" id=\"");
            result += std::to_string(m_id);
            result += "\" name=\"" + m_name + "\"";
            return result;
        }
        
        virtual string NodeType() { return "sym"; }
        virtual void Visit(cVisitor *visitor) { }

    protected:
        static long long nextId;    // keeps track of unique symbol IDs
        long long m_id;             // Unique ID for this symbol
        string m_name;              // Symbol name
        cDeclNode *m_decl;          // Declaration node for this symbol
};
