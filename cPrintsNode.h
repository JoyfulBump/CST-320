#pragma once
//**************************************
// cPrintsNode.h
//
// Defines an AST node for prints statements (string output).
//
// Author: Phil Howard 
// phil.howard@oit.edu
//

#include "cAstNode.h"
#include "cStmtNode.h"
#include <string>

class cPrintsNode : public cStmtNode
{
    public:
        // constructor taking string value
        cPrintsNode(std::string *str) : cStmtNode()
        {
            // Remove quotes from the string
            m_string = str->substr(1, str->length() - 2);
            delete str;
        }

        virtual string AttributesToString() 
        {
            return " value=\"" + m_string + "\"";
        }

        virtual string NodeType() { return string("prints"); }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
    
    protected:
        string m_string;
};
