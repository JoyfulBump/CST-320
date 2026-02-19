#include "cSymbolTable.h"
#include "cSymbol.h"
#include "cBaseTypeNode.h"
#include <unordered_map>
#include <vector>


using std::string;
using std::unordered_map;
using std::vector;

// Define the static member
long long cSymbol::nextId = 0;

// Each scope is a hash table
using SymbolMap = unordered_map<string, cSymbol*>;

// Stack of scopes (inner-most is back)
static vector<SymbolMap*> g_scopeStack;

cSymbolTable::cSymbolTable()
{
    IncreaseScope(); // global scope
    
    // Initialize built-in types in order: char, int, float, long, double
    // char: 1 byte, not float
    cBaseTypeNode *charTypeNode = new cBaseTypeNode("char", 1, false);
    cSymbol *charType = new cSymbol("char");
    charType->SetDecl(charTypeNode);
    Insert(charType);
    
    // int: 4 bytes, not float
    cBaseTypeNode *intTypeNode = new cBaseTypeNode("int", 4, false);
    cSymbol *intType = new cSymbol("int");
    intType->SetDecl(intTypeNode);
    Insert(intType);
    
    // float: 4 bytes, is float
    cBaseTypeNode *floatTypeNode = new cBaseTypeNode("float", 4, true);
    cSymbol *floatType = new cSymbol("float");
    floatType->SetDecl(floatTypeNode);
    Insert(floatType);
    
    // long: 8 bytes, not float
    cBaseTypeNode *longTypeNode = new cBaseTypeNode("long", 8, false);
    cSymbol *longType = new cSymbol("long");
    longType->SetDecl(longTypeNode);
    Insert(longType);
    
    // double: 8 bytes, is float
    cBaseTypeNode *doubleTypeNode = new cBaseTypeNode("double", 8, true);
    cSymbol *doubleType = new cSymbol("double");
    doubleType->SetDecl(doubleTypeNode);
    Insert(doubleType);
}


symbolTable_t *cSymbolTable::IncreaseScope()
{
    SymbolMap *scope = new SymbolMap();
    g_scopeStack.push_back(scope);
    return reinterpret_cast<symbolTable_t*>(scope);
}


symbolTable_t *cSymbolTable::DecreaseScope()
{
    if (!g_scopeStack.empty())
    {
        g_scopeStack.pop_back(); 
    }

    if (g_scopeStack.empty())
        return nullptr;

    return reinterpret_cast<symbolTable_t*>(g_scopeStack.back());
}


void cSymbolTable::Insert(cSymbol *sym)
{
    if (g_scopeStack.empty() || sym == nullptr)
        return;

    (*g_scopeStack.back())[sym->GetName()] = sym;
}

cSymbol *cSymbolTable::Find(std::string name)
{
    for (auto it = g_scopeStack.rbegin(); it != g_scopeStack.rend(); ++it)
    {
        auto found = (*it)->find(name);
        if (found != (*it)->end())
            return found->second;
    }
    return nullptr;
}


cSymbol *cSymbolTable::FindLocal(std::string name)
{
    if (g_scopeStack.empty())
        return nullptr;

    auto found = g_scopeStack.back()->find(name);
    if (found != g_scopeStack.back()->end())
        return found->second;

    return nullptr;
}

// Define the global symbol table
cSymbolTable g_symbolTable;
