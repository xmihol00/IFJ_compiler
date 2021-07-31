
#ifndef __SEMANTIC_PRIVATE_H__
#define __SEMANTIC_PRIVATE_H__

//=========================================================================================================
// File:        semantic_private.h
// Case:        VUT, FIT, IFJ, compiler project
// Date:        9. 12. 2020
// Author(s):   David Mihola
// Contac(s):   xmihol00@stud.fit.vutbr.cz
// Compiled:    gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)
// Description: Header file containing private constant and function declarations for the semantic module.
//==========================================================================================================

#include "semantic.h"

#define CONTROL_STACK_START_SIZE 8
#define CONTROL_STACK_START_POS -1
#define PARAM_BLOCK 0

/**
 * @brief Adds new specific type to the type control.
 *        Can result in error if the type is added in bad context.
 * @param type The new type to be added.
**/
void cadd_type(uint8_t type);

/**
 * @brief Adds the type of a variable in the type cntrol.
 *        Can result in error if the type is added in bad context.
 * @param var The variable which type is to be added.
**/
void cadd_var(htab_var_t var);

/**
 * @brief Adds the return type(s) of a function to the type control.
 *        Can result in error if a function with multiple return types is used in bad context.
 * @param func The function which return type is to be added.
**/
void cadd_func_call(htab_func_t func);

/**
 * @brief Checks if a function is void
 * @param func The function which return type is to be added.
**/
void cadd_void_func(htab_func_t func);

/**
 * @brief Adds a new operator to the type control.
 *        If the operator does not match the inserted types, the addition results in error.
 *        Otherwise the types in type control are changed accordingly.
 * @param op the operator to be added
 * @return The modified operator
**/
void cadd_op(uint8_t op);

/**
 * @brief Assign a type to a variabl after an end of expression, or causes error 
 *        if the variable already has it's type set and the types does not match.
 *        Error is also generated, when no more types are to be assigned.
 * @param var The variable to wihch the result of the expression is assigned. 
**/
void cadd_assgn(htab_var_t var);

/**
 * @brief Sets the type control to a multiple value assignment.
**/
void cadd_multi_assgn_start();

/**
 * @brief Ends the function call and the type control is set to a state which it was in before the function call.
 *        Results in error when the function has more parameters than values that were added up to this point.
 * @param param_count when the function which is to be called is print, then the param_count will be loaded with 
 *        the number of return values
 * @return The function which is to be called
**/
void cadd_fcall_end();

/**
 * @brief Ends the the multiple assignment.
 *        Results in error when there are less variables to be assigned than values that were added up to this point.
**/
void cadd_multi_assgn_end();

/**
 * @brief Sets the type cotrol to check a return value
 * @param function the fanction from which is returned
**/
void cadd_return();

/**
 * @brief Checks if the return values are correct
**/
void cadd_return_end();

/**
 * @brief Checks if the type after the end of expression is boolean
**/
void cadd_cnd_end();

#endif //__SEMANTIC_PRIVATE_H__
