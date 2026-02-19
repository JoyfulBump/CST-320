%{
//**************************************
// lang.y
//
// Parser definition file. bison uses this file to generate the parser.
//
// Author: Phil Howard 
//

#include <iostream>
#include "lex.h"
#include "astnodes.h"

%}

%code requires {
#include <string>
#include "astnodes.h"
}

%locations
 /* union defines the type for lexical values */
%union{
    int             int_val;
    float           float_val;
    std::string*    str_val;
    cAstNode*       ast_node;
    cProgramNode*   program_node;
    cBlockNode*     block_node;
    cStmtsNode*     stmts_node;
    cStmtNode*      stmt_node;
    cExprNode*      expr_node;
    cIntExprNode*   int_node;
    cSymbol*        symbol;
    }

%{

   #define CHECK_ERROR() { if (g_semanticErrorHappened) \
    { g_semanticErrorHappened = false; } }
    #define PROP_ERROR() { if (g_semanticErrorHappened) \
    { g_semanticErrorHappened = false; YYERROR; } }

    int yyerror(const char *msg);
    void SemanticParseError(std::string error);
    extern cSymbolTable g_symbolTable;
    cAstNode *yyast_root;
    static bool g_semanticErrorHappened = false;
%}

%start  program

%token <str_val>   IDENTIFIER
%token <symbol>    TYPE_ID
%token <int_val>   CHAR_VAL
%token <int_val>   INT_VAL
%token <float_val> FLOAT_VAL
%token <int_val>   LE
%token <int_val>   GE
%token <int_val>   AND
%token <int_val>   OR
%token <int_val>   EQUALS
%token <int_val>   NOT_EQUALS
%token <str_val>   STRING_LIT

%token  PROGRAM
%token  PRINT PRINTS
%token  WHILE IF ELSE ENDIF
%token  STRUCT ARRAY
%token  RETURN
%token  JUNK_TOKEN

%type <program_node> program
%type <block_node> block
%type <ast_node> open
%type <ast_node> close
%type <ast_node> decls
%type <ast_node> decl
%type <ast_node> var_decl
%type <ast_node> struct_decl
%type <ast_node> array_decl
%type <ast_node> func_decl
%type <ast_node> func_header
%type <ast_node> func_prefix
%type <ast_node> func_call
%type <ast_node> paramsspec
%type <ast_node> paramspec
%type <stmts_node> stmts
%type <stmt_node> stmt
%type <ast_node> params
%type <ast_node> param
%type <expr_node> expr
%type <expr_node> addit
%type <expr_node> term
%type <expr_node> fact
%type <ast_node> varref
%type <symbol> varpart

%%

program: PROGRAM block
                                { $$ = new cProgramNode($2);
                                  yyast_root = $$;
                                  if (yynerrs == 0) 
                                      YYACCEPT;
                                  else
                                      YYABORT;
                                }
block:  open decls stmts close
                                { $$ = new cBlockNode((cDeclsNode*)$2, $3); }
    |   open stmts close
                                { $$ = new cBlockNode(nullptr, $2); }

open:   '{'
                                { g_symbolTable.IncreaseScope(); }

close:  '}'
                                { g_symbolTable.DecreaseScope(); }

decls:      decls decl
                                { $$ = $1; ((cDeclsNode *)$$)->Insert((cDeclNode *)$2); }
        |   decl
                                { $$ = new cDeclsNode((cDeclNode *)$1); }
decl:       var_decl ';'
                                { $$ = $1; }
        |   array_decl ';'
                                { $$ = $1; }
        |   struct_decl ';'
                                { $$ = $1; }
        |   func_decl
                                { $$ = $1; }
        |   error ';'
                                {  }

var_decl:   TYPE_ID IDENTIFIER
                                    { 
                                      // Check for duplicate definition in current scope
                                      cSymbol* existing = g_symbolTable.FindLocal(*$2);
                                      cSymbol* useSym = nullptr;
                                      
                                      if (existing != nullptr)
                                      {
                                          SemanticParseError("Symbol " + *$2 + " already defined in current scope");
                                          useSym = existing; // Reuse existing to continue parsing
                                      }
                                      else
                                      {
                                          useSym = new cSymbol(*$2);
                                          g_symbolTable.Insert(useSym);
                                      }
                                      
                                      cVarDeclNode* varDecl = new cVarDeclNode($1, useSym);
                                      if (existing == nullptr)
                                      {
                                          useSym->SetDecl(varDecl);
                                      }
                                      $$ = varDecl;
                                      delete $2;
                                      CHECK_ERROR();
                                    }
struct_decl:  STRUCT open decls close IDENTIFIER
                                { 
                                  // Check for duplicate definition in current scope
                                  cSymbol* existing = g_symbolTable.FindLocal(*$5);
                                  cSymbol* useSym = nullptr;
                                  
                                  if (existing != nullptr)
                                  {
                                      SemanticParseError("Symbol " + *$5 + " already defined in current scope");
                                      useSym = existing; // Reuse existing to continue parsing
                                  }
                                  else
                                  {
                                      useSym = new cSymbol(*$5);
                                      g_symbolTable.Insert(useSym);
                                  }
                                  
                                  cStructDeclNode* structDecl = new cStructDeclNode((cDeclsNode*)$3, useSym);
                                  if (existing == nullptr)
                                  {
                                      useSym->SetDecl(structDecl);
                                  }
                                  $$ = structDecl;
                                  delete $5;
                                  CHECK_ERROR();
                                }
array_decl:   ARRAY TYPE_ID '[' INT_VAL ']' IDENTIFIER
                                { 
                                  // Check for duplicate definition in current scope
                                  cSymbol* existing = g_symbolTable.FindLocal(*$6);
                                  cSymbol* useSym = nullptr;
                                  
                                  if (existing != nullptr)
                                  {
                                      SemanticParseError("Symbol " + *$6 + " already defined in current scope");
                                      useSym = existing; // Reuse existing to continue parsing
                                  }
                                  else
                                  {
                                      useSym = new cSymbol(*$6);
                                      g_symbolTable.Insert(useSym);
                                  }
                                  
                                  cArrayDeclNode* arrayDecl = new cArrayDeclNode($2, $4, useSym);
                                  if (existing == nullptr)
                                  {
                                      useSym->SetDecl(arrayDecl);
                                  }
                                  $$ = arrayDecl;
                                  delete $6;
                                  CHECK_ERROR();
                                }

func_decl:  func_header ';'
                                { 
                                  cFuncDeclNode* funcDecl = (cFuncDeclNode*)$1;
                                  
                                  // If there's an existing function with body, copy it to this declaration
                                  cSymbol* funcSym = funcDecl->GetFuncSymbol();
                                  if (funcSym != nullptr)
                                  {
                                      cSymbol* existing = g_symbolTable.Find(funcSym->GetName());
                                      if (existing != nullptr && existing->IsFunc() && existing->GetDecl() != funcDecl)
                                      {
                                          cFuncDeclNode* existingFunc = dynamic_cast<cFuncDeclNode*>(existing->GetDecl());
                                          if (existingFunc != nullptr)
                                          {
                                              funcDecl->CopyBodyFrom(existingFunc);
                                          }
                                      }
                                  }
                                  
                                  $$ = $1; 
                                  g_symbolTable.DecreaseScope(); 
                                }
        |   func_header  '{' decls stmts '}'
                                { 
                                  cFuncDeclNode* funcDecl = (cFuncDeclNode*)$1;
                                  
                                  funcDecl->AddChild($3);
                                  funcDecl->AddChild($4);
                                  
                                  // Check for multiple definitions and update symbol
                                  cSymbol* funcSym = funcDecl->GetFuncSymbol();
                                  if (funcSym != nullptr)
                                  {
                                      cSymbol* existing = g_symbolTable.Find(funcSym->GetName());
                                      if (existing != nullptr && existing->IsFunc())
                                      {
                                          cFuncDeclNode* existingFunc = dynamic_cast<cFuncDeclNode*>(existing->GetDecl());
                                          if (existingFunc != nullptr && existingFunc->HasBody() && existingFunc != funcDecl)
                                          {
                                              SemanticParseError(funcSym->GetName() + " already has a definition");
                                          }
                                      }
                                      
                                      // Always update symbol to point to the definition with body
                                      if (existing != nullptr)
                                      {
                                          existing->SetDecl(funcDecl);
                                      }
                                  }
                                  
                                  $$ = $1;
                                  g_symbolTable.DecreaseScope();
                                }
        |   func_header  '{' stmts '}'
                                { 
                                  cFuncDeclNode* funcDecl = (cFuncDeclNode*)$1;
                                  
                                  funcDecl->AddChild($3);
                                  
                                  // Check for multiple definitions and update symbol
                                  cSymbol* funcSym = funcDecl->GetFuncSymbol();
                                  if (funcSym != nullptr)
                                  {
                                      cSymbol* existing = g_symbolTable.Find(funcSym->GetName());
                                      if (existing != nullptr && existing->IsFunc())
                                      {
                                          cFuncDeclNode* existingFunc = dynamic_cast<cFuncDeclNode*>(existing->GetDecl());
                                          if (existingFunc != nullptr && existingFunc->HasBody() && existingFunc != funcDecl)
                                          {
                                              SemanticParseError(funcSym->GetName() + " already has a definition");
                                          }
                                      }
                                      
                                      // Always update symbol to point to the definition with body
                                      if (existing != nullptr)
                                      {
                                          existing->SetDecl(funcDecl);
                                      }
                                  }
                                  
                                  $$ = $1;
                                  g_symbolTable.DecreaseScope();
                                }
func_header: func_prefix paramsspec ')'
                                { 
                                  cFuncDeclNode* newFunc = (cFuncDeclNode*)$1;
                                  newFunc->AddChild($2);
                                  
                                  // Get the symbol name
                                  cSymbol* funcSym = newFunc->GetFuncSymbol();
                                  if (funcSym != nullptr)
                                  {
                                      cSymbol* existing = g_symbolTable.Find(funcSym->GetName());
                                      if (existing != nullptr && existing->IsFunc() && existing->GetDecl() != newFunc)
                                      {
                                          // Validate against existing function
                                          cFuncDeclNode* existingFunc = dynamic_cast<cFuncDeclNode*>(existing->GetDecl());
                                          if (existingFunc != nullptr)
                                          {
                                              // Check return type
                                              if (existingFunc->GetType() != newFunc->GetType())
                                              {
                                                  SemanticParseError(funcSym->GetName() + " previously declared with different return type");
                                              }
                                              
                                              // Check parameter count
                                              if (existingFunc->GetNumParams() != newFunc->GetNumParams())
                                              {
                                                  SemanticParseError(funcSym->GetName() + " redeclared with a different number of parameters");
                                              }
                                          }
                                      }
                                  }
                                  
                                  $$ = $1;
                                }
        |    func_prefix ')'
                            { 
                              cFuncDeclNode* newFunc = (cFuncDeclNode*)$1;
                              
                              // Get the symbol name
                              cSymbol* funcSym = newFunc->GetFuncSymbol();
                              if (funcSym != nullptr)
                              {
                                  cSymbol* existing = g_symbolTable.Find(funcSym->GetName());
                                  if (existing != nullptr && existing->IsFunc() && existing->GetDecl() != newFunc)
                                  {
                                      // Validate against existing function
                                      cFuncDeclNode* existingFunc = dynamic_cast<cFuncDeclNode*>(existing->GetDecl());
                                      if (existingFunc != nullptr)
                                      {
                                          // Check return type
                                          if (existingFunc->GetType() != newFunc->GetType())
                                          {
                                              SemanticParseError(funcSym->GetName() + " previously declared with different return type");
                                          }
                                          
                                          // Check parameter count
                                          if (existingFunc->GetNumParams() != newFunc->GetNumParams())
                                          {
                                              SemanticParseError(funcSym->GetName() + " redeclared with a different number of parameters");
                                          }
                                      }
                                  }
                              }
                              
                              $$ = $1; 
                            }
func_prefix: TYPE_ID IDENTIFIER '('
                                { 
                                  cSymbol* existingLocal = g_symbolTable.FindLocal(*$2);
                                  cSymbol* existingAny = g_symbolTable.Find(*$2);
                                  cSymbol* useSym = nullptr;
                                  cFuncDeclNode* funcDecl = nullptr;
                                  bool shouldSetDecl = false;
                                  
                                  if (existingLocal != nullptr)
                                  {
                                      // Symbol exists in current scope - check if it's a function
                                      if (!existingLocal->IsFunc())
                                      {
                                          SemanticParseError("Symbol " + *$2 + " already defined in current scope");
                                          // Still create new symbol to continue
                                          useSym = new cSymbol(*$2);
                                          g_symbolTable.Insert(useSym);
                                          shouldSetDecl = true;
                                      }
                                      else
                                      {
                                          // It's a function in current scope - reuse it
                                          useSym = existingLocal;
                                          shouldSetDecl = false; // Don't update decl yet - will be done later
                                      }
                                  }
                                  else if (existingAny != nullptr)
                                  {
                                      // Symbol exists in outer scope - just reuse it, don't insert or update decl
                                      useSym = existingAny;
                                      shouldSetDecl = false;
                                  }
                                  else
                                  {
                                      // No symbol exists at all - create new one
                                      useSym = new cSymbol(*$2);
                                      g_symbolTable.Insert(useSym);
                                      shouldSetDecl = true;
                                  }
                                  
                                  funcDecl = new cFuncDeclNode($1, useSym);
                                  
                                  // Only set decl for brand new symbols
                                  if (shouldSetDecl)
                                  {
                                      useSym->SetDecl(funcDecl);
                                  }
                                  
                                  g_symbolTable.IncreaseScope();
                                  $$ = funcDecl;
                                  delete $2;
                                  CHECK_ERROR();
                                }
paramsspec:  paramsspec ',' paramspec
                                { 
                                  $$ = $1;
                                  ((cParamListNode*)$$)->Insert((cDeclNode*)$3);
                                }
        |   paramspec
                            { $$ = new cParamListNode((cDeclNode*)$1); }

paramspec:  var_decl
                                    { $$ = $1; }

stmts:      stmts stmt
                                { ((cStmtsNode*)$1)->Insert((cStmtNode*)$2); $$ = $1; }
        |   stmt
                            { $$ = new cStmtsNode((cStmtNode*)$1); }

stmt:       IF '(' expr ')' stmts ENDIF ';'
                                { $$ = new cIfNode($3, $5); }
        |   IF '(' expr ')' stmts ELSE stmts ENDIF ';'
                                { $$ = new cIfNode($3, $5, $7); }
        |   WHILE '(' expr ')' stmt
                                { $$ = new cWhileNode($3, $5); }
        |   PRINT '(' expr ')' ';'
                                { $$ = new cPrintNode($3); }
        |   PRINTS '(' STRING_LIT ')' ';'
                                { $$ = new cPrintsNode($3); }
        |   varref '=' expr ';'
                            { $$ = new cAssignNode($1, $3); }
        |   func_call ';'
                            { $$ = (cStmtNode*)$1; }
        |   block
                            { $$ = (cStmtNode*)$1; }
        |   RETURN expr ';'
                            { $$ = new cReturnNode($2); }
        |   error ';'
                            {}

func_call:  IDENTIFIER '(' params ')'
                                    { 
                                      cSymbol* sym = g_symbolTable.FindLocal(*$1);
                                      if (sym == nullptr) {
                                          sym = g_symbolTable.Find(*$1);
                                          if (sym != nullptr) {
                                              // Symbol exists in outer scope - create local copy
                                              sym = new cSymbol(*$1);
                                              g_symbolTable.Insert(sym);
                                          } else {
                                              SemanticParseError("Symbol " + *$1 + " not defined");
                                              sym = new cSymbol(*$1);
                                              g_symbolTable.Insert(sym);
                                          }
                                      }
                                      CHECK_ERROR();
                                      $$ = new cFuncCallNode(sym, $3);
                                      delete $1;
                                    }
        |   IDENTIFIER '('  ')'
                            { 
                              cSymbol* sym = g_symbolTable.FindLocal(*$1);
                              if (sym == nullptr) {
                                  sym = g_symbolTable.Find(*$1);
                                  if (sym != nullptr) {
                                      // Symbol exists in outer scope - create local copy
                                      sym = new cSymbol(*$1);
                                      g_symbolTable.Insert(sym);
                                  } else {
                                      SemanticParseError("Symbol " + *$1 + " not defined");
                                      sym = new cSymbol(*$1);
                                      g_symbolTable.Insert(sym);
                                  }
                              }
                              CHECK_ERROR();
                              $$ = new cFuncCallNode(sym);
                              delete $1;
                            }

varref:   varref '.' varpart
                                { 
                                  // Add the member symbol to the existing varref
                                  ((cVarExprNode*)$1)->AddSymbol($3);
                                  $$ = $1;
                                }
        | varref '[' expr ']'
                            { 
                              ((cVarExprNode*)$1)->AddChild($3);
                              $$ = $1;
                            }
        | varpart
                            { $$ = new cVarExprNode($1); }

varpart:  IDENTIFIER
                                { 
                                  $$ = g_symbolTable.Find(*$1);
                                  if ($$ == nullptr) {
                                      SemanticParseError("Symbol " + *$1 + " not defined");
                                      $$ = new cSymbol(*$1);
                                  }
                                  delete $1;
                                  CHECK_ERROR();
                                }

params:   params ',' param
                                { 
                                  $$ = $1;
                                  ((cParamsNode*)$$)->Insert((cExprNode*)$3);
                                }
        |   param
                            { $$ = new cParamsNode((cExprNode*)$1); }

param:      expr
                                { $$ = $1; }

expr:       expr EQUALS addit
                                { $$ = new cBinaryExprNode($1, new cOpNode(EQUALS), $3); }
        |   addit
                            { $$ = $1; }

addit:      addit '+' term
                                { $$ = new cBinaryExprNode($1, new cOpNode('+'), $3); }
        |   addit '-' term
                            { $$ = new cBinaryExprNode($1, new cOpNode('-'), $3); }
        |   term
                            { $$ = $1; }

term:       term '*' fact
                                { $$ = new cBinaryExprNode($1, new cOpNode('*'), $3); }
        |   term '/' fact
                            { $$ = new cBinaryExprNode($1, new cOpNode('/'), $3); }
        |   term '%' fact
                            { $$ = new cBinaryExprNode($1, new cOpNode('%'), $3); }
        |   fact
                            { $$ = $1; }

fact:       '(' expr ')'
                                { $$ = $2; }
        |   INT_VAL
                            { $$ = new cIntExprNode($1); }
        |   FLOAT_VAL
                            { $$ = new cFloatExprNode($1); }
        |   varref
                            { $$ = (cExprNode*)$1; }
        |   func_call
                            { $$ = (cExprNode*)$1; }

%%

// Function to format error messages
int yyerror(const char *msg)
{
    std::cerr << "ERROR: " << msg << " at symbol "
        << yytext << " on line " << yylineno << "\n";

    return 0;
}

// Function that gets called when a semantic error happens
void SemanticParseError(std::string error)
{
    std::cout << "ERROR: " << error << " near line " 
              << yylineno << "\n";
    g_semanticErrorHappened = true;
    yynerrs++;
}

