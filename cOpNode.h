#pragma once
//**************************************
// cOpNode.h
//
// Class for an operator (e.g. + - etc.)
//
// Author: Phil Howard 
// phil.howard@oit.edu
//
// Date: Jan. 18, 2016
//

#include "cAstNode.h"
#include "langparse.h"

class cOpNode : public cAstNode
{
    public:
        cOpNode(int op) : cAstNode()
        {
            m_op = op;
        }

        int GetOp() { return m_op; }

        virtual string NodeType()       { return "op"; }
        virtual void Visit(cVisitor *visitor) { visitor->Visit(this); }
        string GetOpAsString()
        {
            string result("");

            switch (m_op)
            {
                case 268:  // NOT_EQUALS
                    result += "!=";
                    break;
                case 267:  // EQUALS
                    result += "==";
                    break;
                case 266:  // OR
                    result += "||";
                    break;
                case 265:  // AND
                    result += "AND";
                    break;
                case 264:  // GE
                    result += "GE";
                    break;
                case '>':
                    result += "GT";
                    break;
                case 263:  // LE
                    result += "LE";
                    break;
                case '<':
                    result += "LT";
                    break;
                default:
                    result += (char)m_op;
                    break;
            }

            return result;
        }
        virtual string AttributesToString()   
        { 
            string result(" value='");

            result += GetOpAsString();

            result += "'";

            return result;
        }
    protected:
        int m_op;      // the operand
};

