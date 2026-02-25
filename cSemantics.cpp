//**************************************
// cSemantics.cpp
//
// Implementation of semantic checking visitor for Lab 5b
//
// Author: Ethan Chinander
//

#include "cSemantics.h"
#include "astnodes.h"
#include <iostream>

// Check assignment type compatibility
void cSemantics::Visit(cAssignNode *node)
{
    // Visit children first
    node->VisitAllChildren(this);
    
    // Get lval and rval using public getters
    cAstNode *lval = node->GetLVal();
    cExprNode *rval = node->GetRVal();
    
    if (lval == nullptr || rval == nullptr) return;
    
    // Get types
    cDeclNode *lvalType = nullptr;
    cDeclNode *rvalType = rval->GetType();
    
    // Check if lval is a varref or other expression
    cVarExprNode *varRef = dynamic_cast<cVarExprNode*>(lval);
    if (varRef != nullptr)
    {
        lvalType = varRef->GetType();
    }
    else
    {
        cExprNode *lvalExpr = dynamic_cast<cExprNode*>(lval);
        if (lvalExpr != nullptr)
        {
            lvalType = lvalExpr->GetType();
        }
    }
    
    if (lvalType == nullptr || rvalType == nullptr) return;
    
    // Check compatibility
    if (!lvalType->IsCompatibleWith(rvalType))
    {
        std::string lvalTypeName = lvalType->GetName();
        std::string rvalTypeName = rvalType->GetName();
        
        node->SemanticError("Cannot assign " + rvalTypeName + " to " + lvalTypeName);
    }
}

// Check binary expression types
void cSemantics::Visit(cBinaryExprNode *node)
{
    // Visit children first
    node->VisitAllChildren(this);
    
    // Type checking is done in GetType() method
    // Just ensure it can determine a type
    node->GetType();
}

// Check for illegal usage of functions as variables and array access
void cSemantics::Visit(cVarExprNode *node)
{
    // Visit children first
    node->VisitAllChildren(this);
    
    // Get the first symbol (the base of the varref)
    cSymbol *firstSym = dynamic_cast<cSymbol*>(node->GetChildAt(0));
    if (firstSym == nullptr) return;
    
    // Check if it's a function being used as a variable
    if (firstSym->IsFunc())
    {
        node->SemanticError("Symbol " + firstSym->GetName() + " is a function, not a variable");
        return;
    }
    
    // Check array access: if there are more than 1 child, check for proper array references
    int numChildren = node->GetNumChildren();
    if (numChildren > 1)
    {
        // Walk through children to check array references
        cSymbol *currentSym = firstSym;
        
        for (int i = 1; i < numChildren; i++)
        {
            cAstNode *child = node->GetChildAt(i);
            
            // If child is an expression, it's an array index
            cExprNode *indexExpr = dynamic_cast<cExprNode*>(child);
            if (indexExpr != nullptr)
            {
                // Parent must be an array
                bool isArray = false;
                if (currentSym != nullptr)
                {
                    cDeclNode *symDecl = currentSym->GetDecl();
                    if (symDecl != nullptr)
                    {
                        // Check if the declaration itself is an array
                        if (symDecl->IsArray())
                        {
                            isArray = true;
                        }
                        // Or if it's a variable whose type is an array
                        else if (symDecl->IsVar())
                        {
                            cVarDeclNode *varDecl = dynamic_cast<cVarDeclNode*>(symDecl);
                            if (varDecl != nullptr)
                            {
                                cDeclNode *typeDecl = varDecl->GetTypeDecl();
                                if (typeDecl != nullptr && typeDecl->IsArray())
                                {
                                    isArray = true;
                                }
                            }
                        }
                    }
                    
                    if (!isArray)
                    {
                        node->SemanticError(currentSym->GetName() + " is not an array");
                    }
                }
                
                // Index must be an int  
                cDeclNode *indexType = indexExpr->GetType();
                if (indexType != nullptr && !indexType->IsInt() && !indexType->IsChar())
                {
                    node->SemanticError("Index of " + currentSym->GetName() + " is not an int");
                }
            }
            
            // Update currentSym for next iteration (for struct members)
            cSymbol *nextSym = dynamic_cast<cSymbol*>(child);
            if (nextSym != nullptr)
            {
                currentSym = nextSym;
            }
        }
    }
}

// Check function calls
void cSemantics::Visit(cFuncCallNode *node)
{
    // Visit children first
    node->VisitAllChildren(this);
    
    cSymbol *funcSym = node->GetFuncSymbol();
    if (funcSym == nullptr) return;
    
    cFuncDeclNode *funcDecl = dynamic_cast<cFuncDeclNode*>(funcSym->GetDecl());
    if (funcDecl == nullptr) return;
    
    // Check 1: Function must be fully defined (has body)
    if (!funcDecl->HasBody())
    {
        node->SemanticError("Function " + funcSym->GetName() + " is not fully defined");
    }
    
    // Check 2: Number of arguments must match
    int expectedParams = funcDecl->GetNumParams();
    int actualArgs = node->GetNumArgs();
    
    if (expectedParams != actualArgs)
    {
        node->SemanticError(funcSym->GetName() + " called with wrong number of arguments");
        return; // Don't check types if count is wrong
    }
    
    // Check 3: Argument types must be compatible
    cParamsNode *actualParams = node->GetParams();
    if (actualParams != nullptr && expectedParams > 0)
    {
        // Get formal parameters from function declaration
        cParamListNode *formalParams = funcDecl->GetParamList();
        if (formalParams != nullptr)
        {
            for (int i = 0; i < expectedParams; i++)
            {
                // Get actual argument type
                cExprNode *actualArg = actualParams->GetParam(i);
                if (actualArg == nullptr) continue;
                
                cDeclNode *actualType = actualArg->GetType();
                if (actualType == nullptr) continue;
                
                // Get formal parameter type
                cDeclNode *formalParam = formalParams->GetDecl(i);
                if (formalParam == nullptr) continue;
                
                cDeclNode *formalType = formalParam->GetType();
                if (formalType == nullptr) continue;
                
                // Check if argument type is compatible with parameter type
                if (!formalType->IsCompatibleWith(actualType))
                {
                    node->SemanticError(funcSym->GetName() + " called with incompatible argument");
                    break; // Only report first incompatible argument
                }
            }
        }
    }
}

// Uncomment and implement Visit methods as needed for semantic checking

/*
void cSemantics::Visit(cArrayDeclNode *node)
{
    node->VisitAllChildren(this);
}

void cSemantics::Visit(cAssignNode *node)
{
    node->VisitAllChildren(this);
}

void cSemantics::Visit(cAstNode *node)
{
    node->VisitAllChildren(this);
}

void cSemantics::Visit(cBaseTypeNode *node)
{
    node->VisitAllChildren(this);
}

void cSemantics::Visit(cBinaryExprNode *node)
{
    node->VisitAllChildren(this);
}

void cSemantics::Visit(cBlockNode *node)
{
    node->VisitAllChildren(this);
}

void cSemantics::Visit(cDeclNode *node)
{
    node->VisitAllChildren(this);
}

void cSemantics::Visit(cDeclsNode *node)
{
    node->VisitAllChildren(this);
}

void cSemantics::Visit(cExprNode *node)
{
    node->VisitAllChildren(this);
}

void cSemantics::Visit(cFloatExprNode *node)
{
    node->VisitAllChildren(this);
}

void cSemantics::Visit(cFuncCallNode *node)
{
    node->VisitAllChildren(this);
}

void cSemantics::Visit(cFuncDeclNode *node)
{
    node->VisitAllChildren(this);
}

void cSemantics::Visit(cIfNode *node)
{
    node->VisitAllChildren(this);
}

void cSemantics::Visit(cIntExprNode *node)
{
    node->VisitAllChildren(this);
}

void cSemantics::Visit(cOpNode *node)
{
    node->VisitAllChildren(this);
}

void cSemantics::Visit(cParamListNode *node)
{
    node->VisitAllChildren(this);
}

void cSemantics::Visit(cParamsNode *node)
{
    node->VisitAllChildren(this);
}

void cSemantics::Visit(cPrintNode *node)
{
    node->VisitAllChildren(this);
}

void cSemantics::Visit(cPrintsNode *node)
{
    node->VisitAllChildren(this);
}

void cSemantics::Visit(cProgramNode *node)
{
    node->VisitAllChildren(this);
}

void cSemantics::Visit(cReturnNode *node)
{
    node->VisitAllChildren(this);
}

void cSemantics::Visit(cStmtNode *node)
{
    node->VisitAllChildren(this);
}

void cSemantics::Visit(cStmtsNode *node)
{
    node->VisitAllChildren(this);
}

void cSemantics::Visit(cStructDeclNode *node)
{
    node->VisitAllChildren(this);
}

void cSemantics::Visit(cSymbol *node)
{
    node->VisitAllChildren(this);
}

void cSemantics::Visit(cVarDeclNode *node)
{
    node->VisitAllChildren(this);
}

void cSemantics::Visit(cVarExprNode *node)
{
    node->VisitAllChildren(this);
}

void cSemantics::Visit(cWhileNode *node)
{
    node->VisitAllChildren(this);
}
*/
