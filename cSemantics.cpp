//**************************************************************
// cSemantics.cpp
//
// Implementation of semantic checking visitor
//

#include "cSemantics.h"
#include "astnodes.h"

void cSemantics::Visit(cAssignNode *node)
{
    node->VisitAllChildren(this);

    cExprNode *lhs = dynamic_cast<cExprNode*>(node->GetLVal());
    cExprNode *rhs = node->GetRVal();

    if (lhs == nullptr || rhs == nullptr) return;

    cDeclNode *lhsType = lhs->GetType();
    cDeclNode *rhsType = rhs->GetType();

    if (lhsType == nullptr || rhsType == nullptr) return;

    // Check type compatibility: can rhsType be assigned to lhsType?
    if (!lhsType->IsCompatibleWith(rhsType))
    {
        // Get types for error message
        cDeclNode *lhsBaseType = lhsType->GetType();
        cDeclNode *rhsBaseType = rhsType->GetType();
        std::string lhsName = lhsBaseType ? lhsBaseType->GetName() : "unknown";
        std::string rhsName = rhsBaseType ? rhsBaseType->GetName() : "unknown";
        node->SemanticError("Cannot assign " + rhsName + " to " + lhsName);
    }
}

void cSemantics::Visit(cBinaryExprNode *node)
{
    node->VisitAllChildren(this);
}

void cSemantics::Visit(cFuncCallNode *node)
{
    node->VisitAllChildren(this);

    cSymbol *funcSym = node->GetFuncSymbol();
    if (funcSym == nullptr) return;

    cDeclNode *decl = funcSym->GetDecl();
    if (decl == nullptr) return;

    cFuncDeclNode *funcDecl = dynamic_cast<cFuncDeclNode*>(decl);
    if (funcDecl == nullptr) return;

    // Check if function has a body (is fully defined)
    if (!funcDecl->HasBody())
    {
        node->SemanticError(funcSym->GetName() + " is not fully defined");
    }
}

void cSemantics::Visit(cVarExprNode *node)
{
    node->VisitAllChildren(this);
}
