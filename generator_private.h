
#ifndef __GENERATOR_PRIVATE_H__
#define __GENERATOR_PRIVATE_H__

//=========================================================================================================
// File:        generator_private.h
// Case:        VUT, FIT, IFJ, compiler project
// Date:        9. 12. 2020
// Author(s):   David Mihola, Vladislav Sokolovskii
// Contac(s):   xmihol00@stud.fit.vutbr.cz, xsokol15@stud.fit.vutbr.cz
// Compiled:    gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)
// Description: Header file containing declaration of functions private for the generator module.
//==========================================================================================================

#include "generator.h"

/**
 * @brief Generates header of the file and defines compiler variables
*/
void generate_header();

/**
 * @brief Generates function header
*/
void generate_func_header();

/**
 * @brief Generates multiple assign using stack
 */
void generate_mulassign(uint8_t);

/**
 *@brief Generates jumps and labels, ensures the correctness of the labels in nested conditions
 */
void generate_if_jumps();

/**
 * @brief Changes the if-else counters and pushes their ordinal number to the stacks
 */
void generate_new_if_block();

/**
 * @brief Generates for loop label and jumps
 */
void generate_for_loop();

/**
 * @brief Generates appropriate return from a function 
**/
void generate_return();

/**
 * @brief Skips a block of token stream that is not necessary for generator in the chosen mode (-O/-)
 * @param stop_token The token that indicates that skipping should be stopped
 */
void skip_token_block(uint8_t stop_token);

/**
 * @brief Generates passing parameters to the function and function call
 */
void generate_function_call();

void try_generate_print();

void generate_idf();

void generate_bool();

void generate_int();

void generate_string();

void generate_float();

#endif //__GENERATOR_PRIVATE_H__