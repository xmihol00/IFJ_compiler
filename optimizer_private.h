
#ifndef __OPTIMIZER_PRIVATE_H__
#define __OPTIMIZER_PRIVATE_H__

//=========================================================================================================
// File:        optimizer_private.h
// Case:        VUT, FIT, IFJ, compiler project
// Date:        9. 12. 2020
// Author(s):   David Mihola
// Contac(s):   xmihol00@stud.fit.vutbr.cz
// Compiled:    gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)
// Description: Header file containing private function declarations of the optimizer module.
//==========================================================================================================

#include "optimizer.h"

/**
 * @brief Predefines non recursive function parameters to the program heaeder 
 **/ 
void define_global_func_params();

/**
 * @brief Optimizes an assign expression to a variable
 * @param end_type the token type present at the end of the expression
 **/
void optimize_adress_assign(uint8_t end_type);

/**
 * @brief Optimizes an conditional expression
 * @param end_type the token type present at the end of the expression
 **/
void optimize_condition(uint8_t end_type);

/**
 * @brief Optimizes a multi assign expression
 **/
void optimize_multi_assgn();

/**
 * @brief Tries to optimize a multi assign expression and if possible calls optimize_multi_assgn()
 **/
void try_multi_assgn_opt();

/**
 * @brief Tries to optimize an assign expression and if possible calls optimize_address_assign()
 **/
void try_assgn_opt();

/**
 * @brief Tries to optimize a conditional expression and if possible calls optimize_condition(uint8_t end_type)
 * @param end_type the type of the token, where the expression ends
 **/
void try_cnd_opt(uint8_t end_type);

/**
 * @brief Optimizes a function call (recursive)
 **/
void optimize_func_call();

/**
 * @brief Optimizes a built-in function call
 * @param called_function the called function to be optimized
 * @param last_step the flag of the previously done optimization
 * @return true when it is an end of a function call, otherwise false
 **/
bool optimize_build_in_fcall(htab_func_t called_function, bool last_step);

/**
 * @brief Optimizes a built-in function call directy to instructions
 * @param called_function the called function to be optimized
 * @param last_step the flag of the previously done optimization
 **/
void optimize_func_to_inst(uint8_t instruction, bool last_step);


/**
 * @brief Optimizes the input built-in functions
 * @param type the type of the input function
 **/
void optimize_input(uint8_t type);

/**
 * @brief Optimizes a return from a function
 **/
void optimize_return();

#endif
