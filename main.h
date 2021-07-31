
#ifndef __MAIN_H__
#define __MAIN_H__

//=========================================================================================================
// File:        main.h
// Case:        VUT, FIT, IFJ, compiler project
// Date:        9. 12. 2020
// Author(s):   David Mihola, Lukas Foltyn, Vladislav Sokolovskii
// Contac(s):   xmihol00@stud.fit.vutbr.cz, xfolty17@stud.fit.vutbr.cz, xsokol15@stud.fit.vutbr.cz
// Compiled:    gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)
// Description: Header file for the main.c file
//==========================================================================================================

#include <stdio.h>

#include "vector.h"
#include "parser.h"
#include "symtable.h"
#include "enums.h"
#include "token.h"
#include "generator.h"
#include "semantic.h"
#include "optimizer.h"
#include "register.h"
#include "oRPN.h"

void init_program();

void dispose_program();

#endif // __MAIN_H__
