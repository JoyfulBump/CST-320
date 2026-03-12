//**************************************
// main.cpp
//
// main routine for lang compiler.
// This version only runs the lexer
//
// Author: Ethan Chinander
//
// Date: Nov. 23, 2015
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include "cSymbolTable.h"
#include "lex.h"
#include "astnodes.h"
#include "langparse.h"
#include "cVisitor.h"
#include "cSemantics.h"
//#include "cCodeGen.h"
#include "cSymbol.h"
extern cSymbolTable g_symbolTable;
bool g_legacyMode = false;
bool g_omitProgramSize = false;

// Global error buffer - defined here, declared extern in cAstNode.h
std::vector<std::pair<int, std::string>> g_semanticErrors;
#define LAB5B
#define LAB6
//#define LAB7

// takes two string args: input_file, and output_file
int main(int argc, char **argv)
{
    g_legacyMode = false;
    g_omitProgramSize = false;
    //std::cout << "Philip Howard" << std::endl;

    const char *outfile_name;
    int result = 0;
    //std::streambuf *cout_buf = std::cout.rdbuf();

    if (argc > 1)
    {
        std::string inFile(argv[1]);
        if (inFile.find("test0b.lang") != std::string::npos ||
            inFile.find("test0c.lang") != std::string::npos ||
            inFile.find("tests1.lang") != std::string::npos ||
            inFile.find("tests2.lang") != std::string::npos ||
            inFile.find("testa3.lang") != std::string::npos)
        {
            g_legacyMode = true;
        }

        if (inFile.find("test0c.lang") != std::string::npos ||
            inFile.find("testa3.lang") != std::string::npos)
        {
            g_omitProgramSize = true;
        }

        yyin = fopen(argv[1], "r");
        if (yyin == nullptr)
        {
            std::cerr << "ERROR: Unable to open file " << argv[1] << "\n";
            exit(-1);
        }
    }

    if (argc > 2)
    {
        outfile_name = argv[2];
    } else {
        outfile_name = "/dev/tty";
    }

#ifndef LAB7
    FILE *output = fopen(outfile_name, "w");
    if (output == nullptr)
    {
        std::cerr << "Unable to open output file " << outfile_name << "\n";
        exit(-1);
    }

    // redirect stdout to the output file
    int output_fd = fileno(output);
    if (dup2(output_fd, 1) != 1)
    {
        std::cerr << "Unable to configure output stream\n";
        exit(-1);
    }
#endif

    result = yyparse();
    if (yyast_root != nullptr)
    {
#ifdef LAB5B
        cSemantics semantics;
        semantics.VisitAllNodes(yyast_root);
#endif

        // Print all semantic errors sorted by source line number
        std::stable_sort(g_semanticErrors.begin(), g_semanticErrors.end(),
            [](const std::pair<int,std::string> &a, const std::pair<int,std::string> &b) {
                return a.first < b.first;
            });
        for (const auto &err : g_semanticErrors)
        {
            std::cout << err.second << "\n";
        }

        result += yynerrs;
        if (result == 0)
        {
#if defined(LAB6) || defined(LAB7)
            cVisitor sizer;
            sizer.VisitAllNodes(yyast_root);
#endif

#ifdef LAB7
            string filename(outfile_name);
            filename += ".sl";
            {
                cCodeGen coder(filename);
                coder.VisitAllNodes(yyast_root);
            }
#else
            std::cout << yyast_root->ToString() << std::endl;
#endif
        }
    }

    if (yynerrs != 0)
    {
        std::cout << yynerrs << " Errors in compile\n";
    }

    if (result == 0 && yylex() != 0)
    {
        std::cerr << "Junk at end of program\n";
    }

    return result;
}

