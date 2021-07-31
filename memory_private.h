#ifndef __MEMORY_PRIVATE_H__
#define __MEMORY_PRIVATE_H__

//=========================================================================================================
// File:        memory_private.h
// Case:        VUT, FIT, IFJ, compiler project
// Date:        9. 12. 2020
// Author(s):   David Mihola
// Contac(s):   xmihol00@stud.fit.vutbr.cz
// Compiled:    gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)
// Description: Header file containing definitions of private constatns and functions.
//==========================================================================================================

#include "memory.h"

#define MMU_START_SIZE 64LU
#define MMU_START_LOAD 0LU /// The starting position is 0 and never can be lower as the structure uses unsigned int
#define MMU_REHASH_VAL 0.625

/**
 * @brief Performs the memory management unit resizing and rehashing, the size is always doubled.
**/
void MMU_rehash();

#endif