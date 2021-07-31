
#ifndef __REGISTER_H__
#define __REGISTER_H__

//=========================================================================================================
// File:        register.h
// Case:        VUT, FIT, IFJ, compiler project
// Date:        9. 12. 2020
// Author(s):   David Mihola
// Contac(s):   xmihol00@stud.fit.vutbr.cz
// Compiled:    gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)
// Description: Header file containing declarations of functions accessible for other modules.
//==========================================================================================================

#define REG_START_SIZE 8
#define REG_START_POS -1
typedef char reg_t[16];

#include "vector.h"
#include "enums.h"

void init_register();

void dispose_register();

/**
 * @brief gets not used register
 * @retiurn not currently used register
**/
char *reg_get();

/**
 * @brief gets the first register as an accumulator
 * @return the first register
**/
char *reg_get_indirect();

/**
 * @brief resets all registers
**/ 
void reg_reset();

/**
 * @brief gets the last created register
 * @return the last register, that was created
**/
char *reg_get_last();

/**
 * @brief resets last register
**/
void reg_set_emtpy();

/**
 * @brief crete definition of used registers in the IFJ20code language
 * @param prog_header string containing the header of the programm
**/
void define_used_registers(string_t *prog_header);

#endif
