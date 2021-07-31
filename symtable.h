
#ifndef __SYMTABLE_H__
#define __SYMTABLE_H__

//=========================================================================================================
// File:        symtable.h
// Case:        VUT, FIT, IFJ, compiler project
// Date:        9. 12. 2020
// Author(s):   David Mihola, Lukas Foltyn
// Contac(s):   xmihol00@stud.fit.vutbr.cz, xfolty17@stud.fit.vutbr.cz
// Compiled:    gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)
// Description: Header file containing declaration of functions and data types used in the table of symbols.
//==========================================================================================================

typedef struct var_htab_item * htab_var_t; //variable
typedef struct func_htab_item * htab_func_t; //function

#include "enums.h"

/**
 * @union possible values which can be assigned to a variable
**/
typedef union
{
    int64_t integer;
    double float64;
    char *string;
    bool boolean;
}var_values_t;

#include "token.h"
#include "vector.h"
#include "scanner.h"
#include <string.h>
#include <stdbool.h>


/* ------------------------------------ HashTable - variables -------------------------------*/
/**
 * @brief initialize a global hash table by allocating a memory for partial hash tables
 **/
void init_global_ht();

/**
 * @brief inserts an element into a currenlty used pertial hash table
 * @param key is name of variable and used for hashing
 * @param scope determines in which scope the variable was declared
 * @return an inserted element
 **/
htab_var_t insert_var_ht(const char* key, const int scope);

htab_var_t get_tmp_var(int64_t pos);

/**
 * @brief finds the first element matching both hash and the key in hash table
 * @param key is name of variable and used for hashing
 * @return a found element or NULL if nothing was found 
 **/
htab_var_t find_var_ht(const char* key);

/**
 * @brief finds the next alement in the hash table, which has the same key and is defined
 * @param var the variable which holds the key
 * @return defined varieble or NULL if nothing is found 
 **/
htab_var_t find_next_def_var(htab_var_t var);

/**
 * @brief frees all the memory allocated for hash tables, that work with variables
 **/
void dispose_global_ht();

/**
 * @brief sets the variables's type
 * @param t is funnction structure
 * @param type is type of variable
 **/
void set_var_type(htab_var_t t, uint8_t type);

/**
 * @brief creates a vector for remembering the hashes of given scope
 * @param started_scope determines which scope(currently started) the function should work with
**/ 
void start_scope(const uint32_t started_scope);

/**
 * @brief rotates (and sets their scope to -1) the variables from given scope, so they can no longer be accesed from lower scopes
 * @param ended_scope determines which scope(currently ended) the function should work with
**/
void end_scope(const int64_t ended_scope);

/**
 * @param t is a variable ht structure
 * @return var_type_t - type of variable
 **/
uint8_t get_var_type(htab_var_t t);

int get_var_scope(htab_var_t var);

/**
 * @brief sets the variable's state
 * @param t is a variable ht structure
 * @param state is state of the variable
 **/
void set_var_state(htab_var_t t, uint8_t state);

/**
 * @param t is a a variable ht structure
 * @return state_t - state of variable
 **/
uint8_t get_var_state(htab_var_t t);

/**
 * @brief sets has_value to true if variable has some value
 * @param t is a variable ht structure
 * @param has_value - determines whether the variable has value or not
 **/
void set_var_has_value(htab_var_t t, bool has_value);

/**
 * @param t is a a variable ht structure
 * @return bool - determines whether the variable has value or not
 **/
bool get_var_has_value(htab_var_t t);

/**
 * @brief add a usage to a variable to a vector
 * @param t is a variable ht structure
 * @param usage - is usage of given variable
 **/
void add_var_usage(htab_var_t t, uint8_t usage);

/**
 * @param t is a a variable ht structure
 * @return vect_uint8_t - vector of cases where the variable is used
 **/
vect_uint8_t get_var_usage(htab_var_t t);

/**
 * @brief Sets the frame of a variable
 * @param var the variable to be modified
 * @param frame the frame of the variable: true - GF@, false - LF@ (TF@ is not used)
 **/
void set_var_frame(htab_var_t var, bool frame);

/**
 * @brief Returns a frame of a variable
 * @param var the variable to be examined
 * @return the frame of the given variable: true - GF@, false - LF@ (TF@ is not used)
 **/
bool get_var_frame(htab_var_t var);

/**
 * @brief Unsets value of an variable, if it has eny, otherwise no effect
 * @param var the variable to be unsetted
 **/
void unset_var_value(htab_var_t var);

/**
 * @brief Gets the value of a given variable, which must have value
 * @return the value of the given variable
 **/
var_values_t get_var_value(htab_var_t var);

/**
 * @brief Assignes a variable with a given value
 * @param var the assigned variable
 * @param value the assigned value
 * @param type the type of the assigned value
 **/
void set_var_value(htab_var_t var, var_values_t value, uint8_t type);

/**
 * @brief Assignes variables with constan values after the end of scope
 **/
void assign_constants_after_scope();

/**
 * @brief Copies the value of all variables with value, when opening new scope
 **/
void copy_vars_to_new_scope();

/**
 * @brief Copies the value of all variables from the previous scope
 **/
void copy_vars_reopened_scope();

/**
 * @brief Removes values of variables, that has changed when a scope is closed
 **/
void remove_vars_values_in_scope();

/**
 * @brief Gets wheater a variable has value or not
 * @param var the variable to be examined
 * @return treu when the variable has value, otherwise false
 **/
bool has_var_value(htab_var_t var);

/**
 * @brief Assignes variables, that have constant value a have not been already assigned
 * @param var the variable to be examined
 * @return treu when the variable has value, otherwise false
 **/
void assign_constants_before_scope();

/**
 * @brief returns the value of variable
 * @param t pointer to a variable struct
 * @return int64_t
**/ 
int64_t get_var_value_integer(htab_var_t t);

/**
 * @brief returns the value of variable
 * @param t pointer to a variable struct
 * @return double
**/ 
double get_var_value_float(htab_var_t t);

/**
 * @brief returns the value of variable
 * @param t pointer to a variable struct
 * @return bool
**/ 
bool get_var_value_boolean(htab_var_t t);

/**
 * @brief returns the value of variable
 * @param t pointer to a variable struct
 * @return const char * 
**/ 
const char *get_var_value_string(htab_var_t t);

/**
 * @brief returns the name of variable
 * @param t pointer to a variable struct
 * @return const char * 
**/ 
const char*get_var_name(htab_var_t t);

/**
 * @brief returns true if the variable is special symbol '_';
 * @param t pointer to a variable struct
 * @return bool
**/
bool is_special_var(htab_var_t t);


/* -----------------------------------HashTable - functions -------------------------------------*/

/**
 * @brief initialize a hash table by allocating a memory
 **/
void init_func_ht();

/**
 * @brief Checks if all functions are defined and function main has the right signiture, 
 *        exits with appropriate exit code on error
**/
void all_func_have_definition();

/**
 * @brief inserts an element into a hash table
 * @param key is name of variable and used for hashing
 * @return an inserted element
 **/
htab_func_t insert_func_ht(const char* key);

/**
 * @brief finds the first element matching both hash and the key in hash table
 * @param key is name of variable and used for hashing
 * @return a found element or NULL is nothing was found 
 **/
htab_func_t find_func_ht(const char* key);

/**
 * @brief frees all the memory allocated by hash table
 **/
void dispose_func_ht();

/**
 *@brief adds a partial htab to a previously declared function
 *@param t is pointer to a function structure located in func hash table 
 **/
void attache_partial_htab(htab_func_t t);

/**
 * @brief sets the function's state
 * @param t is function structure
 * @param state is state of the function
 **/
void set_func_state(htab_func_t t , uint8_t state);

/**
 * @brief sets the variable for side effects
 * @param t is a function structure
 * @param visited determines whether the function has some side effects or not
 **/
void set_func_effect(htab_func_t t, bool visited);

/**
 * @brief sets the variable for recursion
 * @param t is a function structure
 * @param is_not_recursive determines whether the function is recursive or not
 **/
void set_func_recursion(htab_func_t t, bool is_not_recursive);

/**
 * @brief add a parameter to a function
 * @param t is a function structure
 * @param v is a variable that is added as a parameter
 **/
void add_func_parameter(htab_func_t t, htab_var_t v);

/**
 * @brief add a type to the last added function parameter
 * @param t is the function
 * @param type is the added type
**/
void add_func_param_type(htab_func_t t, uint8_t type);

/**
 * @brief add a return value to a function
 * @param t is a function structure
 * @param v is a a variable that is added as a return value 
 **/
void add_func_ret_val(htab_func_t t, htab_var_t v);

/**
 * @brief add a type to the last added function return value
 * @param t the function
 * @param type the added type
 * @param has_name indicates if the return value has name
**/
void add_func_ret_val_type(htab_func_t t, uint8_t type, bool has_name);

/**
 * @brief determines weather a function has named or not named return types
 * @param t the function
 * @return NAMED_S - if the return types are named
 *         NOT_NAMED_D - if the return types aren't named
 *         NOT_SET_S - if it is not determined yet
**/
uint8_t get_func_ret_val_signature(htab_func_t t);

/**
 * @brief return function's paramters
 * @param t is a function structure
 * @return vect_htab_var_t - an array of function's parameters
 **/
vect_htab_var_t get_params(htab_func_t t);

/**
 * @brief return function's return values
 * @param t is a function structure
 * @return vect_htab_var_t - an array of function's return values
 **/
vect_htab_var_t get_ret_vals(htab_func_t t);

/**
 * @brief return function's return values types
 * @param t is a function structure
 * @return vector of function return values or null, when there are no return values
 **/
vect_uint8_t get_ret_val_types(htab_func_t t);

/**
 * @param t is a function structure
 * @return uint8, which determines a state of function
 **/
uint8_t get_func_state(htab_func_t t);

/**
 * @param t is a function structure
 * @return bool - true if function has some side effects
 **/
bool get_func_effect(htab_func_t t);

/**
 * @param t is a function structure
 * @return bool - true if function is recursive
 **/
bool func_is_recursive(htab_func_t t);

bool func_is_not_recursive(htab_func_t t);

/**
 * @brief returns the name of func
 * @param t pointer to a func struct
 * @return const char * 
**/ 
const char*get_func_name(htab_func_t t);

/**
 * @brief Adds a function call to a function
 * @param f the calle, where the function call is assigned
 * @param called the called function 
**/ 
void add_func_call(htab_func_t f, htab_func_t called);

/**
 * @brief Marks all recursive functions
**/ 
void find_recursive_functions();

/* --------------------- "HashTable" - keywords -------------------- */

/**
 * @brief takes an string argument and tries to match it with keyword
 * @return uint8t - type of enum that matches given keyword, otherwise 0 is returned
 **/	
uint8_t is_key_word(const char *word);

/**
 * @brief determines wheater a function is built-in
 * @return uint8t - type of enum that matches given build in function, otherwise 0 is returned
 **/	
uint8_t is_build_in_function(htab_func_t func);

/**
 * @brief determines wheater a function can be converted to an instruction
 * @return uint8t - type of enum that matches given build in function, otherwise 0 is returned
 **/	
uint8_t is_function_to_instruction(htab_func_t func);

/**
 * @brief determines wheater a function is input{i,f,s,b}
 * @return uint8t - type of enum that matches given build in function, otherwise 0 is returned
 **/
uint8_t is_input_function(htab_func_t func);

/* ------------------- test functions ------------------ */

/**
 * @brief prints all elements from all hash tables
 **/
void print_all();

/**
 * @brief prints the name of a given variable
 **/ 
void print_var_name(htab_var_t t);

/**
 * @brief Returns the name of a given variable
 **/ 
char *read_var_name(htab_var_t t);

/**
 * @brief Returns the name of a given function
 **/ 
char *read_func_name(htab_func_t f);

/**
 * @brief Returns a string with a default value of a variable (int@0, bool@false, ...).
 * @param var the variable of which the default value is returned.
 * @return The default value.
**/
char *get_var_def_value(htab_var_t var);

#endif
