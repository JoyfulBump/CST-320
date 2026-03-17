//**************************************************************
// cComputeSize.cpp
//
// Visitor to compute sizes of variables and blocks
//
// Author: Phil Howard
// Email: phil.howard@oit.edu
//
//**************************************************************

#include "cComputeSize.h"
#include "astnodes.h"
#include "lex.h"

// Helper function to align offsets
static int Align(int offset, int alignment)
{
    if (alignment <= 1) return offset;
    return ((offset + alignment - 1) / alignment) * alignment;
}

// Stub implementations for all visit methods
// Most just visit children, some do actual size computation

void cComputeSize::Visit(cProgramNode *node)
{
    if (node == nullptr) return;
    
    cBlockNode *block = node->GetBlock();
    if (block != nullptr)
    {
        block->Visit(this);
        // Program size = block size
        if (block->GetSize() > 0)
        {
            node->SetSize(block->GetSize());
        }
    }
}

void cComputeSize::Visit(cBlockNode *node)
{
    if (node == nullptr) return;
    
    // Save the high water mark at entry to this block
    int savedHighWaterMark = m_highWaterMark;
    
    // Visit children (m_offset and m_highWaterMark will be updated by decls/stmts)
    node->VisitAllChildren(this);
    
    // Block size is the increase in high water mark
    int blockSize = m_highWaterMark - savedHighWaterMark;
    node->SetSize(blockSize);
}

void cComputeSize::Visit(cDeclsNode *node)
{
    if (node == nullptr) return;
    
    int startOffset = m_offset;
    
    for (int i = 0; i < node->GetNumDecls(); i++)
    {
        cDeclNode *child = node->GetDecl(i);
        if (child != nullptr)
        {
            // If it's a variable declaration, compute its offset and size
            if (child->IsVar())
            {
                cVarDeclNode *varDecl = static_cast<cVarDeclNode*>(child);
                
                // Get the type size
                int typeSize = 0;
                cDeclNode *typeDecl = varDecl->GetTypeDecl();
                if (typeDecl != nullptr)
                {
                    typeSize = typeDecl->GetSize();
                    // In legacy mode, float variables occupy 8 bytes
                    if (g_legacyMode && typeDecl->IsFloat() && typeSize == 4)
                    {
                        typeSize = 8;
                    }
                }
                
                // Align the offset (anything with size > 1 must be 4-byte aligned)
                int alignment = (typeSize > 1) ? 4 : 1;
                m_offset = Align(m_offset, alignment);
                
                // Set offset and size
                varDecl->SetOffset(m_offset);
                varDecl->SetSize(typeSize);
                
                // Update offset and high water mark
                m_offset += typeSize;
                if (m_offset > m_highWaterMark)
                {
                    m_highWaterMark = m_offset;
                }
            }
            
            // Visit the child
            child->Visit(this);
        }
    }
    
    // Set decls size to the amount used by these decls
    node->SetSize(m_offset - startOffset);
}

void cComputeSize::Visit(cVarDeclNode *node)
{
    if (node == nullptr) return;
    // Size and offset already set by cDeclsNode visitor
}

void cComputeSize::Visit(cFuncDeclNode *node)
{
    if (node == nullptr) return;
    
    // Save previous offset and high water mark
    int savedOffset = m_offset;
    int savedHighWaterMark = m_highWaterMark;
    
    // Reset for function scope
    m_offset = 0;
    m_highWaterMark = 0;
    
    // Process parameters
    cParamListNode *params = node->GetParamList();
    int totalParamSize = 0;
    
    if (params != nullptr)
    {
        if (g_legacyMode)
        {
            // Legacy mode: params start at offset 0, go positive sequentially
            int paramOffset = 0;
            
            for (int i = 0; i < params->GetNumDecls(); i++)
            {
                cDeclNode *paramDecl = params->GetDecl(i);
                if (paramDecl != nullptr)
                {
                    int paramSize = paramDecl->GetSize();
                    // In legacy mode, float params get size 8
                    cVarDeclNode *varDecl = dynamic_cast<cVarDeclNode*>(paramDecl);
                    if (varDecl != nullptr)
                    {
                        cDeclNode *typeDecl = varDecl->GetTypeDecl();
                        if (typeDecl != nullptr && typeDecl->IsFloat() && paramSize == 4)
                            paramSize = 8;
                    }
                    int alignment = (paramSize > 1) ? 4 : 1;
                    paramOffset = Align(paramOffset, alignment);
                    paramDecl->SetOffset(paramOffset);
                    paramDecl->SetSize(paramSize);
                    paramOffset += paramSize;
                    totalParamSize = paramOffset;
                }
            }
            
            params->SetSize(totalParamSize);
            
            // In legacy mode, locals start after params
            m_offset = totalParamSize;
            m_highWaterMark = totalParamSize;
        }
        else
        {
            // Non-legacy mode: params have negative offsets starting at -12
            int paramOffset = -12;
            
            for (int i = 0; i < params->GetNumDecls(); i++)
            {
                cDeclNode *paramDecl = params->GetDecl(i);
                if (paramDecl != nullptr)
                {
                    int paramSize = paramDecl->GetSize();
                    paramDecl->SetOffset(paramOffset);
                    // Parameters are always 4-byte aligned on the stack
                    int alignedSize = Align(paramSize, 4);
                    paramOffset -= alignedSize;
                    totalParamSize += alignedSize;
                }
            }
            
            params->SetSize(totalParamSize);
        }
    }
    
    // Visit all other children (params will be visited by VisitAllChildren)
    node->VisitAllChildren(this);
    
    // Function size: legacy = params+locals, non-legacy = locals only
    int funcSize = Align(m_highWaterMark, m_highWaterMark > 1 ? 4 : 1);
    node->SetSize(funcSize);
    
    // Restore previous state
    m_offset = savedOffset;
    m_highWaterMark = savedHighWaterMark;
}

void cComputeSize::Visit(cParamListNode *node)
{
    if (node == nullptr) return;
    // Size is already set by cFuncDeclNode
    // Just visit children (but don't recalculate size like cDeclsNode does)
    for (int i = 0; i < node->GetNumDecls(); i++)
    {
        cDeclNode *child = node->GetDecl(i);
        if (child != nullptr)
        {
            child->Visit(this);
        }
    }
}

void cComputeSize::Visit(cStmtsNode *node)
{
    if (node == nullptr) return;
    
    // Save block start
    int savedHighWaterMark = m_highWaterMark;
    
    // Visit each statement
    for (int i = 0; i < node->GetNumStmts(); i++)
    {
        cStmtNode *stmt = node->GetStmt(i);
        if (stmt != nullptr)
        {
            stmt->Visit(this);
        }
        // Reset high water mark after each statement (local scopes end)
        m_highWaterMark = savedHighWaterMark;
    }
}

// Stub implementations for other visit methods that just visit children
void cComputeSize::Visit(cArrayDeclNode *node)     { if(node) node->VisitAllChildren(this); }
void cComputeSize::Visit(cAssignNode *node)        { if(node) node->VisitAllChildren(this); }
void cComputeSize::Visit(cAstNode *node)           { if(node) node->VisitAllChildren(this); }
void cComputeSize::Visit(cBaseTypeNode *node)      { if(node) node->VisitAllChildren(this); }
void cComputeSize::Visit(cBinaryExprNode *node)    { if(node) node->VisitAllChildren(this); }
void cComputeSize::Visit(cDeclNode *node)          { if(node) node->VisitAllChildren(this); }
void cComputeSize::Visit(cExprNode *node)          { if(node) node->VisitAllChildren(this); }
void cComputeSize::Visit(cFloatExprNode *node)     { if(node) node->VisitAllChildren(this); }
void cComputeSize::Visit(cFuncCallNode *node)      { if(node) node->VisitAllChildren(this); }
void cComputeSize::Visit(cIfNode *node)            { if(node) node->VisitAllChildren(this); }
void cComputeSize::Visit(cIntExprNode *node)       { if(node) node->VisitAllChildren(this); }
void cComputeSize::Visit(cOpNode *node)            { if(node) node->VisitAllChildren(this); }
void cComputeSize::Visit(cParamsNode *node)        { if(node) node->VisitAllChildren(this); }
void cComputeSize::Visit(cPrintNode *node)         { if(node) node->VisitAllChildren(this); }
void cComputeSize::Visit(cPrintsNode *node)        { if(node) node->VisitAllChildren(this); }
void cComputeSize::Visit(cReturnNode *node)        { if(node) node->VisitAllChildren(this); }
void cComputeSize::Visit(cStmtNode *node)          { if(node) node->VisitAllChildren(this); }
void cComputeSize::Visit(cStructDeclNode *node)    { if(node) node->VisitAllChildren(this); }
void cComputeSize::Visit(cSymbol *node)            { }
void cComputeSize::Visit(cVarExprNode *node)       { if(node) node->VisitAllChildren(this); }
void cComputeSize::Visit(cWhileNode *node)         { if(node) node->VisitAllChildren(this); }
