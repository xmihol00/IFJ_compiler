#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__

//=========================================================================================================
// File:        functions.h
// Case:        VUT, FIT, IFJ, compiler project
// Date:        9. 12. 2020
// Author(s):   Lukas Foltyn
// Contac(s):   xfolty17@stud.fit.vutbr.cz
// Compiled:    gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)
// Description: Declaration of functions generating target IFJ20code instructions of built-in functions.
//==========================================================================================================

#include "enums.h"

void c_input(uint8_t type);

void c_print();

void c_int2float();

void c_float2int();

void c_len();

void c_substr();

void c_ord();

void c_chr();

char *get_func_data_type(uint8_t func_type);

#endif //__FUNCTIONS_H__
