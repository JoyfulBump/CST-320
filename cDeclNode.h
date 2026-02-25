#pragma once
//**************************************
// cDeclNode
//
// Defines base class for all declarations.
// Future labs will add features to this class.
//
// Author: Phil Howard 
// phil.howard@oit.edu
//

#include "cAstNode.h"

class cDeclNode : public cAstNode
{
    public:
        cDeclNode() : cAstNode() {}
        
        // Type identification methods
        virtual bool IsArray()  { return false; }
        virtual bool IsStruct() { return false; }
        virtual bool IsType()   { return false; }
        virtual bool IsFunc()   { return false; }
        virtual bool IsVar()    { return false; }
        virtual bool IsFloat()  { return false; }
        virtual bool IsInt()    { return false; }
        virtual bool IsChar()   { return false; }
        virtual int  GetSize()  { return 0; }
        virtual cDeclNode *GetType() = 0;
        virtual string GetName() = 0;
        
        // Type compatibility checking
        // Returns true if 'type' can be assigned to 'this'
        virtual bool IsCompatibleWith(cDeclNode *type)
        {
            if (type == nullptr || this == nullptr) return false;
            
            // Get base types
            cDeclNode *thisType = this->GetType();
            cDeclNode *otherType = type->GetType();
            
            if (thisType == nullptr || otherType == nullptr) return false;
            
            // Same type is always compatible
            if (thisType == otherType) return true;
            
            // Check categories
            bool thisIsFloat = thisType->IsFloat();
            bool otherIsFloat = otherType->IsFloat();
            
            // Can't mix int and float without promotion rules
            // Integer can promote to float, but float can't demote to int
            if (thisIsFloat && !otherIsFloat)
            {
                // Assigning int to float - OK (promotion)
                return true;
            }
            else if (!thisIsFloat && otherIsFloat)
            {
                // Assigning float to int - ERROR
                return false;
            }
            
            // Both same category (both int or both float)
            // Check size - can only assign smaller or equal to larger
            int thisSize = thisType->GetSize();
            int otherSize = otherType->GetSize();
            
            // Can assign if source size <= dest size
            return otherSize <= thisSize;
        }
        
        // Decls are their own decl
        virtual cDeclNode *GetDecl() { return this; }
};
