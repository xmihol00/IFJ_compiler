
#ifndef __MEMORY_H__
#define __MEMORY_H__

//=========================================================================================================
// File:        memory.h
// Case:        VUT, FIT, IFJ, compiler project
// Date:        9. 12. 2020
// Author(s):   David Mihola
// Contac(s):   xmihol00@stud.fit.vutbr.cz
// Compiled:    gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)
// Description: Header file containing function declarations of accessible function for othe modules.
//==========================================================================================================

#include "enums.h"

/**
 * @brief Initilazes the memory management unit.
**/
void init_MMU();

/**
 * @brief Destructs (frees) all memory pointed to by the MMU and aslo the MMU itself
**/
void dispose_MMU();

/**
 * @brief Adds a new pointer to a memory which needs to be watched. The addition is done by using a simple hash
 *        algorithm. The algorithm finds a starting position in the array by modulating the pointer value by the
 *        MMU size and then finding the first empty spot. The table is resized and rehashed currently when the
 *        load factor reaches 0.625.
 * @param ptr Pointer to a memory to be added for watching by the MMU.
**/
void MMU_add(void *ptr);

/**
 * @brief Finds the pointer which needs to be discarded (freed) by the same hashing algorithm and discards it. 
 * @param ptr Pointer to a memory, that is going to be discarded (freed)
**/
void MMU_discard(void *ptr);

/**
 * @brief Reallocates the memory of an stored item in the MMU to a new size
 * @param ptr Pointer to the memory, which is to be reallocated
 * @param new_size The new size of the memory
 * @return The adress of the newly allocated memory (in case of memory reallocation failure, the program ends befor return)
**/
void *MMU_realloc_item(void *ptr, uint64_t new_size);


#endif
