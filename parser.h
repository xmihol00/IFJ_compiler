
#ifndef __PARSER_H__
#define __PARSER_H__

//=========================================================================================================
// File:        parser.h
// Case:        VUT, FIT, IFJ, compiler project
// Date:        9. 12. 2020
// Author(s):   David Mihola
// Contac(s):   xmihol00@stud.fit.vutbr.cz
// Compiled:    gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)
// Description: Header file containing function declarations accessible for other modules.
//==========================================================================================================

#include "enums.h"
#include "vector.h"
#include "symtable.h"
#include "scanner.h"
#include "oRPN.h"
#include "token.h"

void init_parser();

void dispose_parser();

void syntax_analysis();

bool is_var_assignable();

#endif
