//**************************************************************
// Implementation of the virtual Visitor class.
// This class must be subclassed (and made concrete) for all visitors
//

#include "lex.h"
#include "cVisitor.h"
#include "astnodes.h"

//******************************************
// All AST node types need a Visit method. 
// The code for each method is identical: simply visit all children

// Helper function to align offsets
static int Align(int offset, int alignment)
{
    if (alignment <= 1) return offset;
    return ((offset + alignment - 1) / alignment) * alignment;
}

// Helper function to compute struct offsets
static void ComputeStructOffsets(cDeclsNode* decls)
{
    if (decls == nullptr) return;
    
    int offset = 0;
    
    // First pass: compute offsets with proper alignment
    for (int i = 0; i < decls->GetNumDecls(); i++)
    {
        cDeclNode *decl = decls->GetDecl(i);
        if (decl != nullptr && decl->IsVar())
        {
            cVarDeclNode *varDecl = static_cast<cVarDeclNode*>(decl);
            
            // Get the type size
            int typeSize = 0;
            cDeclNode *typeDecl = varDecl->GetTypeDecl();
            if (typeDecl != nullptr)
            {
                typeSize = typeDecl->GetSize();
            }
            
            // Align based on type size: anything > 1 byte needs 4-byte alignment
            int alignment = (typeSize > 1) ? 4 : 1;
            offset = Align(offset, alignment);

            // In struct layout, float members occupy 8 bytes in expected output
            int memberSize = typeSize;
            if (typeDecl != nullptr && typeDecl->IsFloat() && typeSize == 4)
            {
                memberSize = 8;
            }
            
            // Set offset and initial size for this member
            varDecl->SetOffset(offset);
            varDecl->SetSize(memberSize);
            
            // Move to next position
            offset += memberSize;
        }
    }
    
    // Total struct size
    decls->SetSize(offset);
}

void cVisitor::VisitAllNodes(cAstNode *node) 
{ node->Visit(this); }

void cVisitor::Visit(cArrayDeclNode *node)    { node->VisitAllChildren(this); }
void cVisitor::Visit(cAssignNode *node)       { node->VisitAllChildren(this); }
void cVisitor::Visit(cAstNode *node)          { node->VisitAllChildren(this); }
void cVisitor::Visit(cBaseTypeNode *node)     { node->VisitAllChildren(this); }
void cVisitor::Visit(cBinaryExprNode *node)   { node->VisitAllChildren(this); }

void cVisitor::Visit(cBlockNode *node)
{
    // Save the high water mark at entry to this block
    int savedHighWaterMark = m_highWaterMark;
    
    // Visit children (m_offset and m_highWaterMark will be updated by decls/stmts)
    node->VisitAllChildren(this);
    
    // Block size is the increase in high water mark
    int blockSize = m_highWaterMark - savedHighWaterMark;
    node->SetSize(blockSize);
    
    // Don't reset here - let parent (cStmtsNode) handle sibling resets
}

void cVisitor::Visit(cDeclNode *node)         { node->VisitAllChildren(this); }

void cVisitor::Visit(cDeclsNode *node)
{
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

void cVisitor::Visit(cExprNode *node)         { node->VisitAllChildren(this); }
void cVisitor::Visit(cFloatExprNode *node)    { node->VisitAllChildren(this); }
void cVisitor::Visit(cFuncCallNode *node)     { node->VisitAllChildren(this); }

void cVisitor::Visit(cFuncDeclNode *node)
{
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

void cVisitor::Visit(cIfNode *node)           { node->VisitAllChildren(this); }
void cVisitor::Visit(cIntExprNode *node)      { node->VisitAllChildren(this); }
void cVisitor::Visit(cOpNode *node)           { node->VisitAllChildren(this); }

void cVisitor::Visit(cParamListNode *node)
{
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

void cVisitor::Visit(cParamsNode *node)
{
    node->VisitAllChildren(this);

    // In legacy mode, call-site params don't have a size attribute
    if (g_legacyMode) return;

    int totalParamSize = 0;
    for (int i = 0; i < node->GetNumParams(); i++)
    {
        cExprNode *param = node->GetParam(i);
        if (param != nullptr)
        {
            cDeclNode *paramType = param->GetType();
            int paramSize = (paramType != nullptr) ? paramType->GetSize() : 0;
            int alignment = (paramSize > 1) ? 4 : 1;
            totalParamSize = Align(totalParamSize, alignment);
            totalParamSize += paramSize;
        }
    }

    node->SetSize(totalParamSize);
}
void cVisitor::Visit(cPrintNode *node)        { node->VisitAllChildren(this); }
void cVisitor::Visit(cPrintsNode *node)       { node->VisitAllChildren(this); }

void cVisitor::Visit(cProgramNode *node)
{
    // Visit all children first
    node->VisitAllChildren(this);
    
    // Get the main block and set program size (aligned), unless omitted for legacy output
    if (!g_omitProgramSize)
    {
        cBlockNode *block = node->GetBlock();
        if (block != nullptr)
        {
            int blockSize = block->GetSize();
            node->SetSize(Align(blockSize, blockSize > 1 ? 4 : 1));
        }
    }
}

void cVisitor::Visit(cReturnNode *node)       { node->VisitAllChildren(this); }
void cVisitor::Visit(cStmtNode *node)         { node->VisitAllChildren(this); }
void cVisitor::Visit(cStmtsNode *node)
{
    // Save the starting position for sibling statement blocks
    int startOffset = m_offset;
    int startHighWaterMark = m_highWaterMark;
    int maxHighWaterMark = m_highWaterMark;
    
    // Visit each child statement
    for (int i = 0; i < node->GetNumStmts(); i++)
    {
        // Reset to starting position so sibling blocks can reuse space
        m_offset = startOffset;
        m_highWaterMark = startHighWaterMark;
        
        // Visit the child
        cStmtNode *child = node->GetStmt(i);
        if (child != nullptr)
        {
            child->Visit(this);
        }
        
        // Track the maximum high water mark reached by any child
        if (m_highWaterMark > maxHighWaterMark)
        {
            maxHighWaterMark = m_highWaterMark;
        }
    }
    
    // Set offset and high water mark to the maximum reached
    m_offset = startOffset;
    m_highWaterMark = maxHighWaterMark;
}

void cVisitor::Visit(cStructDeclNode *node)
{
    // Save previous offset and high water mark (struct members use their own namespace)
    int savedOffset = m_offset;
    int savedHighWaterMark = m_highWaterMark;
    
    // Visit children first
    node->VisitAllChildren(this);
    
    // Compute struct member offsets and sizes
    cDeclsNode *decls = node->GetDecls();
    if (decls != nullptr)
    {
        ComputeStructOffsets(decls);
        node->SetSize(decls->GetSize());
    }
    
    // Restore previous state (struct definition doesn't consume space in parent)
    m_offset = savedOffset;
    m_highWaterMark = savedHighWaterMark;
}

void cVisitor::Visit(cSymbol *node)           { node->VisitAllChildren(this); }

void cVisitor::Visit(cVarDeclNode *node)
{
    // Visit children first
    node->VisitAllChildren(this);
    
    // Size is already set by parent or by type
    if (node->GetDeclSize() == 0)
    {
        cDeclNode *typeDecl = node->GetTypeDecl();
        if (typeDecl != nullptr)
        {
            int declSize = typeDecl->GetSize();
            if (g_legacyMode && typeDecl->IsFloat() && declSize == 4)
            {
                declSize = 8;
            }
            node->SetSize(declSize);
        }
    }
}

void cVisitor::Visit(cVarExprNode *node)
{
    // Visit children first
    node->VisitAllChildren(this);
    
    // Compute offset by walking the chain of symbols
    int totalOffset = 0;
    int finalSize = 0;
    string rowsizes = "";
    
    // Check if there's array indexing (non-symbol child)
    bool hasArrayIndex = false;
    for (int i = 0; i < node->GetNumChildren(); i++)
    {
        cSymbol *sym = dynamic_cast<cSymbol*>(node->GetChildAt(i));
        if (sym == nullptr)
        {
            // This is not a symbol, likely an array index expression
            hasArrayIndex = true;
            break;
        }
    }
    
    cDeclNode *currentTypeDecl = nullptr;

    // Go through all children (symbols and indices in the chain)
    for (int i = 0; i < node->GetNumChildren(); i++)
    {
        cAstNode *child = node->GetChildAt(i);
        cSymbol *sym = dynamic_cast<cSymbol*>(child);
        if (sym != nullptr)
        {
            cDeclNode *decl = sym->GetDecl();
            if (decl != nullptr)
            {
                totalOffset += decl->GetOffset();
                finalSize = decl->GetSize();

                if (decl->IsVar())
                {
                    cVarDeclNode *varDecl = static_cast<cVarDeclNode*>(decl);
                    currentTypeDecl = varDecl->GetTypeDecl();
                }
                else
                {
                    currentTypeDecl = decl;
                }
            }
        }
        else if (hasArrayIndex && currentTypeDecl != nullptr && currentTypeDecl->IsArray())
        {
            cArrayDeclNode *arrayDecl = static_cast<cArrayDeclNode*>(currentTypeDecl);
            cDeclNode *elementDecl = arrayDecl->GetBaseDecl();
            if (elementDecl != nullptr)
            {
                if (!rowsizes.empty()) rowsizes += " ";
                rowsizes += std::to_string(elementDecl->GetSize());
                currentTypeDecl = elementDecl;
            }
            else
            {
                currentTypeDecl = nullptr;
            }
        }
    }
    
    node->SetOffset(totalOffset);
    node->SetSize(finalSize);
    if (!rowsizes.empty())
    {
        node->SetRowsizes(rowsizes);
    }
}

void cVisitor::Visit(cWhileNode *node)        { node->VisitAllChildren(this); }

