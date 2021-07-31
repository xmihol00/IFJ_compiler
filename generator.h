
#ifndef __GENERATOR_H__
#define __GENERATOR_H__

//=========================================================================================================
// File:        generator.c
// Case:        VUT, FIT, IFJ, compiler project
// Date:        9. 12. 2020
// Author(s):   David Mihola, Vladislav Sokolovskii
// Contac(s):   xmihol00@stud.fit.vutbr.cz, xsokol15@stud.fit.vutbr.cz
// Compiled:    gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)
// Description: Header file containing declaration of functions accessible for other modules.
//==========================================================================================================

#include "token.h"
#include "semantic.h"
#include "vector.h"
#include "functions.h"
#include "optimizer.h"
#include "enums.h"

/**
 * @brief Initializes the generator structure
*/
void init_generator();

void dispose_generator();

/**
 * @brief Generates target code out of token stream
*/
void generator();

#endif //__GENERATOR_H__
