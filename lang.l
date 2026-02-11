%{
//**************************************
// lang.l
//
// scanner definition file. flex uses this file to create the scanner
//
// Author: <<<< Ethan >>>>
//
#include "langparse.h"
#include "lex.h"
#include "cSymbolTable.h"

// Macros that can be used to create debug output from the scanner
// Uncomment the next line of code to generate debug output.
// Note: for this to work, use DO_RETURN(val) for all the return statements
//       for your tokens. See sample below for "program"
//#define DEBUG_OUTPUT
#ifdef DEBUG_OUTPUT
    #define DO_RETURN(a) { return Return(a); }
#else
    #define DO_RETURN(a) { return (a); }
#endif

int Return(int val);
int Process_ID(const char* text);

%}

%option noyywrap
%option noinput
%option nounput
%option yylineno


    /* definitions go here. period is an example. */
    /* NOTE: You do not need to use definitions */
letters      [a-zA-Z]
digits       [0-9]
ident        [_a-zA-Z][_a-zA-Z0-9]*
int_const    {digits}+
float_const  {digits}+"."{digits}+
string_lit     \"[^\"]*\"
%%

    // token definitions. I gave "program" as an example
"program"/[^_a-zA-Z0-9]      DO_RETURN(PROGRAM)
"if"/[^_a-zA-Z0-9]           DO_RETURN(IF)
"else"/[^_a-zA-Z0-9]         DO_RETURN(ELSE)
"endif"/[^_a-zA-Z0-9]        DO_RETURN(ENDIF)
"while"/[^_a-zA-Z0-9]        DO_RETURN(WHILE)
"print"/[^_a-zA-Z0-9]        DO_RETURN(PRINT)
"prints"/[^_a-zA-Z0-9]       DO_RETURN(PRINTS)
"struct"/[^_a-zA-Z0-9]       DO_RETURN(STRUCT)
"array"/[^_a-zA-Z0-9]        DO_RETURN(ARRAY)
"return"/[^_a-zA-Z0-9]       DO_RETURN(RETURN)


"{"            DO_RETURN('{')
"}"            DO_RETURN('}')

"!="           DO_RETURN(NOT_EQUALS)
"=="           DO_RETURN(EQUALS)
"&&"           DO_RETURN(AND)
"||"           DO_RETURN(OR)
">="           DO_RETURN(GE)
"<="           DO_RETURN(LE)


{int_const}    { yylval.int_val = atoi(yytext); DO_RETURN(INT_VAL); }
{float_const}  { yylval.float_val = atof(yytext); DO_RETURN(FLOAT_VAL); }
{string_lit}   { yylval.str_val = new std::string(yytext); DO_RETURN(STRING_LIT); }

{ident}        { DO_RETURN(Process_ID(yytext)); }

"//".*         { /* skip comment */ }
[ \t\r\n]+     { /* skip */ }

"."            DO_RETURN('.')
"("            DO_RETURN('(')
")"            DO_RETURN(')')
"["            DO_RETURN('[')
"]"            DO_RETURN(']')
","            DO_RETURN(',')
";"            DO_RETURN(';')
"{"            DO_RETURN('{')
"}"            DO_RETURN('}')
"="            DO_RETURN('=')
"+"            DO_RETURN('+')
"-"            DO_RETURN('-')
"/"            DO_RETURN('/')
"*"            DO_RETURN('*')
"%"            DO_RETURN('%')
">"            DO_RETURN('>')
"<"            DO_RETURN('<')

.              { DO_RETURN(JUNK_TOKEN); }


%%


int Process_ID(const char* text)
{
    // Check if this identifier is a known type
    cSymbol* sym = g_symbolTable.Find(text);
    
    if (sym && sym->IsType())
    {
        // It's a type - return the symbol
        yylval.symbol = sym;
        return TYPE_ID;
    }
    else
    {
        // It's a regular identifier - return the name as a string
        yylval.str_val = new std::string(text);
        return IDENTIFIER;
    }
}
// This function allows us to do extra processing on each token
// It is used to generate lexer debug info
int Return(int val)
{
    printf("Scanned '%s': %d\n", yytext, val);
    return val;
}
