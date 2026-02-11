#pragma once
//********************************************************
// cSymbol.h - Define a class for symbols
//
// Author: Philip Howard
//
#include <string>

using std::string;
#include "cAstNode.h"

class cSymbol : public cAstNode
{
    public:
        // Construct a symbol given its name
        cSymbol(string name)
        {
            m_id = ++nextId;
            m_name = name;
            m_isType = false;
        }

        // Return name of symbol
        string GetName() { return m_name; }
        // Return ID of symbol
        long long GetId() { return m_id; }
        
        // Type tracking
        void SetIsType(bool isType) { m_isType = isType; }
        bool IsType() { return m_isType; }

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
        bool m_isType;              // True if this symbol represents a type
};
