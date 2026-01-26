%{
//**************************************
// lang.l
//
// scanner definition file. flex uses this file to create the scanner
//
// Author: Ethan
//

#include "lex.h"
#include "tokens.h"

// Macros that can be used to create debug output from the scanner
// Uncomment the next line of code to generate debug output.
// Note: for this to work, use DO_RETURN(val) for all the return statements
//       for your tokens.
#define DEBUG_OUTPUT
#ifdef DEBUG_OUTPUT
    #define DO_RETURN(a) { return Return(a); }
#else
    #define DO_RETURN(a) { return (a); }
#endif

int Return(int val);

%}

%option noyywrap
%option noinput
%option nounput
%option yylineno

digits       [0-9]
ident        [_a-zA-Z][_a-zA-Z0-9]*
int_const    {digits}+

%%

{int_const}    DO_RETURN(INT_VAL)

{ident}        DO_RETURN(IDENTIFIER)

"("            DO_RETURN(LPAREN)
")"            DO_RETURN(RPAREN)
"["            DO_RETURN(LBRACKET)
"]"            DO_RETURN(RBRACKET)
"{"            DO_RETURN(LBRACE)
"}"            DO_RETURN(RBRACE)
";"            DO_RETURN(SEMI)
"="            DO_RETURN(ASSIGN)
"+"            DO_RETURN(PLUS)
"-"            DO_RETURN(MINUS)
"*"            DO_RETURN(TIMES)
"/"            DO_RETURN(DIVIDE)

[ \t\r\n]+     { /* skip */ }

.              { DO_RETURN(JUNK_TOKEN); }

%%

// This function allows us to do extra processing on each token
// It is used to generate lexer debug info
int Return(int val)
{
    printf("Scanned '%s': %d\n", yytext, val);
    return val;
}