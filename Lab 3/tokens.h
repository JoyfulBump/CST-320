#pragma once

//**************************************
// tokens.h
//
// Defines the values for the tokens returned by yylex
//
// Author: Phil Howard 
// phil.howard@oit.edu
//

#define INT_VAL     1000        // <an integer constant>
#define IDENTIFIER  1001        // <a C style identifier>
#define LPAREN      1002        // (
#define RPAREN      1003        // )
#define LBRACKET    1004        // [
#define RBRACKET    1005        // ]
#define LBRACE      1006        // {
#define RBRACE      1007        // }
#define SEMI        1008        // ;
#define ASSIGN      1009        // =
#define PLUS        1010        // +
#define MINUS       1011        // -
#define TIMES       1012        // *
#define DIVIDE      1013        // /

#define JUNK_TOKEN  2000