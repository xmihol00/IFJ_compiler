#ifndef __OPTIMIZER_H__
#define __OPTIMIZER_H__

//=========================================================================================================
// File:        optimizer.h
// Case:        VUT, FIT, IFJ, compiler project
// Date:        9. 12. 2020
// Author(s):   David Mihola
// Contac(s):   xmihol00@stud.fit.vutbr.cz
// Compiled:    gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)
// Description: Header file containing declarations of functions accessible to other modules.
//==========================================================================================================

#include "register.h"
#include "symtable.h"
#include "token.h"
#include "vector.h"
#include "semantic.h"

void init_optimizer();

void dispose_optimizer();

void optimizer();

/**
 * @brief Returns the header of a program with predefined variables creted by the optimizer 
 * @return NULL if the header is empty, otherwise a valid string
 **/ 
string_t get_program_header();

#endif
