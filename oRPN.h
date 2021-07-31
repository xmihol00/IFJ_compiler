
#ifndef __ORPN_H__
#define __ORPN_H__

//=========================================================================================================
// File:        oRPN.h
// Case:        VUT, FIT, IFJ, compiler project
// Date:        9. 12. 2020
// Author(s):   David Mihola
// Contac(s):   xmihol00@stud.fit.vutbr.cz
// Compiled:    gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)
// Description: Header file containing function declarations accessible for other modules.
//==========================================================================================================

#include "symtable.h"
#include "vector.h"
#include "enums.h"
#include "token.h"
#include "parser.h"

void init_orpn();

void dispose_orpn();

/**
 * @brief signal the end of expression
**/
void orpn_expr_end();

/**
 * @brief add constant to the expression
 * @param type the data type of the constant (int, string, bool or float)
 * @param data the actual data
**/
void orpn_add_value(uint8_t type, token_data_t data);

/**
 * @brief add function to the expression
 * @param func the added function
**/
void orpn_add_func(htab_func_t func);

/**
 * @brief add variable to the expression
 * @param func the added variable
**/
void orpn_add_var(htab_var_t var);

/**
 * @brief add operator to the expression
 * @param func the added operator
**/
void orpn_add_operator(uint8_t operator);

/**
 * @brief add open bracket to the expression
**/
void orpn_add_ob();

/**
 * @brief add close bracket to the expression
 * @return true if the closed bracket ended a function call, otherwise false
**/
bool orpn_add_cb();

/**
 * @brief add variable to which the result of the expression is assigned
 * @param var the assigned variable
**/
void orpn_add_assgn_var(htab_var_t var);

#endif //__ORPN_H__