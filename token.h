

#ifndef __TOKEN_H__
#define __TOKEN_H__

//=========================================================================================================
// File:        token.h
// Case:        VUT, FIT, IFJ, compiler project
// Date:        9. 12. 2020
// Author(s):   David Mihola - functions needed for optimalization, Vladislav Sokolovskii - compulsory functions
// Contac(s):   xfolty17@stud.fit.vutbr.cz, xsokol15@stud.fit.vutbr.cz
// Compiled:    gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)
// Description: Header file containing data type and function declarations accessible for other modules.
//==========================================================================================================

#include "enums.h"

typedef struct token * token_t;
typedef token_t token_couple_t[2];

#include "vector.h"
#include "symtable.h"

typedef union {    ///Union that holds the token data
    char *string;      ///the lenght of the string will be known and can be allocated accordingly
    int64_t integer;   ///the value can be negative -> int64_t
    double float64;    /// floating point value
    bool boolean;      ///boolean variables
    htab_var_t var;    ///variable
    htab_func_t func;  ///function
    uint8_t op;        ///operator
}token_data_t;

/**
 * @brief Initialize the token list structure, after the initialization list already contains two
 * empty nodes. Tail is pointing to the second last character of the list.
 */
void init_list();

/**
 * @brief Deallocates all the memory used by the elements of the token list
 */
void dispose_list();

/**
 * @brief Inserts new token element without the data at the end of the token list
 * @param type Type of the inserted token
 */
void ltype_insert(uint8_t type);

/**
 * @brief Inserts new token with the T_INT type at the end of the token list
 * @param integer Integer value of the newly inserted token
 */
void lint_insert(int64_t integer);

/**
 * @brief Inserts new token with the T_FLOAT type at the end of the token list
 * @param decimal Float64 value of the newly inserted token
 */
void lfloat_insert(double decimal);

/**
 * @brief Inserts new token with the T_BOOL type at the end of the token list
 * @param boolean Bool value of the newly inserted token
 */
void lbool_insert(bool boolean);

/**
 * @brief Inserts new token with the T_STRING type at the end of the token list
 * @param string String value of the newly inserted token
 */
void lstring_insert(char *string);

/**
 * @brief Inserts new token with the T_IDF type at the end of the token list
 * @param variable Variable identifier value of the newly inserted token
 */
void lvar_insert(htab_var_t variable);

/**
 * @brief Inserts new token with the T_FUNC type at the end of the token list
 * @param function Function identifier value of the newly inserted token
 */
void lfunc_insert(htab_func_t function);

/**
 * @brief Inserts new token with the T_OP or T_OP_EQ or T_NOT type at the end of the token list
 * @param op Operator value of the newly inserted token
 */
void lop_insert(uint8_t op);

/**
 * @return Returns the type of the last token in the list
 */
uint8_t lget_last_token();

/**
 * @return Returns the type of the second last token in the list
 */
uint8_t lget_2nd_last_token();

/**
 * @brief Saves the current position of the tail to the saved_tail
 */
void lsave_position();

/**
 * @brief Moves reader before the beginning of the token list (skips one empty node)
 */
void lreader_head();

/**
 * @brief Moves reader to the next token in the list
 */
void lreader_next();

/**
 * @brief Checks if the list is empty
 * @return Returns true if the list is empty, otherwise returns false
 */
bool llist_is_not_empty();

/**
 * @brief Return type and data of the next token but doesn't move the reader to that token
 * @param type Through this variable the type of the next token will be passed
 * @return Returns data of the next token
 */
token_data_t lcheck_next(uint8_t *type);

/**
 * @brief Return type and data of the next token and moves the reader to that token
 * @param type Through this variable the type of the next token will be passed
 * @return Returns data of the next token
 */
token_data_t lread_next(uint8_t *type);

/**
 * @brief Moves the reader in front of the head
**/
void lreader_pre_head();

/**
 * @brief Returns the type of the token pointed by reader
 * @return the token type
**/
uint8_t lreader_type();

/**
 * @brief Returns the type of the next token pointed by reader
 * @return the token type
**/
uint8_t lreader_n_type();

/**
 * @brief Returns the type of the next next token pointed by reader
 * @return the token type
**/
uint8_t lreader_nn_type();

/**
 * @brief Returns the type of the next token pointed by reader and moves the reader
 * @return the token type
**/
uint8_t lreader_move_next_type();

/**
 * @brief Looks for an occurance of a token type until onother type
 * @param type the searched type
 * @param until the token type to what the search is performed
 * @return true when found, otherwise false
**/
bool lreader_find_occurence_until(uint8_t type, uint8_t until);

/**
 * @brief Looks for an occurance of variable until a specific token type
 * @param type the searched variable
 * @param until the token type to what the search is performed
 * @return true when found, otherwise false
**/
bool lreader_find_var_until(htab_var_t var, uint8_t until);

/**
 * @brief Changes the token type pointed by reader
 * @param type the new type
**/
void lreader_change_type(uint8_t type);

/**
 * @brief Changes the next token type pointed by reader
 * @param type the new type
**/
void lreader_change_n_type(uint8_t type);

/**
 * @brief Changes the data of the next token pointed by reader
 * @param data the new data
**/
void lreader_change_n_data(token_data_t data);

/**
 * @brief Returns the data of the token pointed by reader
 * @return the token data
**/
token_data_t lreader_data();

/**
 * @brief Returns the data of the next token pointed by reader
 * @return the token data
**/
token_data_t lreader_n_data();

/**
 * @brief Returns the data of the next next token pointed by reader
 * @return the token data
**/
token_data_t lreader_nn_data();

/**
 * @brief Returns the data of the next next next token pointed by reader
 * @return the token data
**/
token_data_t lreader_nnn_data();

/**
 * @brief Returns the whole token pointed by the reader (the token is unaccessable from other modules)
**/
token_t lreader_token();

/**
 * @brief Returns the whole next token pointed by the reader (the token is unaccessable from other modules)
**/
token_t lreader_n_token();

/**
 * @brief Inserts a new token to the token stream after the token ponited by the reader
 * @param type the new token type
 * @param data the new token data
**/
void lreader_insert_next(uint8_t type, token_data_t data);

/**
 * @brief Replaces the data of next token pointed by the reader
 * @param type the new token type
 * @param data the new token data
**/
void lreader_n_replace(uint8_t type, token_data_t data);

/**
 * @brief Inserts a new token to the token stream at a place, where the reader points
 * @param type the new token type
 * @param data the new token data
**/
void lreader_insert_construct(uint8_t type, token_data_t data);

/**
 * @brief Inserts a new token to the token stream after the next token pointed by the reader
 * @param type the new token type
 * @param data the new token data
**/
void lreader_n_insert_construct(uint8_t type, token_data_t data);

/**
 * @brief Removes next two tokens from the token stream ponited by the reader
**/
void lreader_remove_next_two();

/**
 * @brief Removes the current token from the token stream ponited by the reader
**/
void lreader_remove();

/**
 * @brief Removes next token from the token stream ponited by the reader
**/
void lreader_remove_next();

/**
 * @brief Removes tokens asociated with the function definition
**/
bool lreader_remove_func_def();

/**
 * @brief Skips tokens until an occurenc of another token
 * @param type the type of the token until which the search is performed
**/
void lreader_skip_until(uint8_t type);

/**
 * @brief Checks if the reader is not at the end of the list
**/
bool llist_is_not_last();

/**
 * @brief Saves the current position of the reader to the stack of saved nodes
**/
void lreader_save_pos_to_stack();

/**
 * @brief Loads and pops the saved position from stack and assigns it to the reader
**/
void lreader_load_pos_from_stack();

/**
 * @brief Pops the stack of saved reader nodes without assigning it
**/
void lreader_pop_stack();

/**
 * @brief Saves the position of the reader, but does not push it on a stack, 
 *        the saved position will be rewritten by the next call
**/
void lreader_save_pos();

/**
 * @brief Reloads the previously stored reader position
**/
void lreader_reaload_saved_pos();

/**
 * @brief Inserts a token of given type to the reader
 * @param type the new type
**/
void lreader_type_insert(uint8_t type);

/**
 * @brief Inserts a token to the last saved position of reader
 * @param token the inserted token
**/
void lsaved_pos_insert(token_t token);

/**
 * @brief removes tokens from the list pointed by the reader until a occurance of a given token
 * @param type the token type where the removing ends
**/
void lreader_remove_until(uint8_t type);

/**
 * @brief Finds the next token with data
 * @return the data type of the nex data token
**/
uint8_t lreader_find_next_data_type();

/**
 * @brief Removes tokens from the list pointed by the saved rader until a occurance of a given token
 * @param type the token type where the removing ends
**/
void lsaved_remove_until(uint8_t type);

/**
 * @brief Creates a new token not assigned to any list
 * @param type the token type of the newly created token
 * @param data the data ofthe newly created token
 * @return the newly created token
**/
token_t tcreate_token(uint8_t type, token_data_t data);

/**
 * @brief Creates a copy of given token
 * @param token token to be copied
 * @return the copy of a given token
**/
token_t tcopy_token(token_t token);

/**
 * @brief returns the data of a given token
 * @param token token to be examined
 * @return the data of a given token
**/
token_data_t tget_token_data(token_t token);

/**
 * @brief returns type of a given token
 * @param token token to be examined
 * @return the type of a given token
**/
uint8_t tget_token_type(token_t token);

/**
 * @brief Cretes a new token at position of the saved reader
 * @param type the type o the new token
 * @param data the data of the new token
**/
void lsaved_pos_insert_construct(uint8_t type, token_data_t data);

/**
 * @brief Pops and deallocates memory of one partial list
**/
void lpartial_pop_free();

/**
 * @brief Creates a new partial list
**/
void lpartial_create();

/**
 * @brief Inserts a new token to the partial list in front of the last item
 * @param type the type o the new token
 * @param data the data of the new token
**/
void lpartial_insert_create(uint8_t type, token_data_t data);

/**
 * @brief Inserts a new token to the partial list after the current item
 * @param type the type o the new token
 * @param data the data of the new token
**/
void lpartial_insert_create_after(uint8_t type, token_data_t data);

/**
 * @brief Move the pointer of the current partial list to the end and performs insert there
 * @param type the type o the new token
 * @param data the data of the new token
**/
void lpartial_insert_create_last(uint8_t type, token_data_t data);

/**
 * @brief Insrets already created token to the partial list
 * @param token the newly inserted token, that is not a part of any list
**/
void lpartial_insert(token_t token);

/**
 * @brief Inserts a partial list to the position of the reader in the main list
**/
void lpartial_insert_partial_to_reader();

/**
 * @brief Changes the token type pointed by partial
**/
void lpartial_change_type(uint8_t type);

/**
 * @brief Signilize the token stream, that the syntax analysis is completed
**/
void syntax_done();

/**
 * @brief Parser calls this function when T_FOR3 token occurs
 * the function inserts T_FOR3 token and saves pointer to the 2nd last token
 * @param type T_FOR3 token type
 */
void lfor3_start(uint8_t type);

/**
 * @brief Parser calls this function when T_FOR3_END occurs,
 * the function inserts T_FOR3_END token and saves pointer to that token
 * @param type T_FOR3_END token type
 */
void lfor3_end(uint8_t type);

/**
 * @brief This function is called by the parser when the third part of the for header is empty
 */
void lfor3_empty();

/**
 @brief Parser calls this function when T_FOR_BODY_END token occurs
 * the function inserts T_FOR_BODY_END token and changes pointers so that FOR3 part goes at the end
 * of the for body
 @param type T_FOR_BODY_END token type
 */
void lfor_body_end(uint8_t type);

/**
 * @brief Prints out the token list for debug
 */
void print_list();

/**
 * @brief Changes the token type of the last element in the list
 * @param type New token type
 */
void lchange_last(uint8_t type);

/**
 * @brief Stores the start of a for cycle
**/
void save_first_for();

/**
 * @brief Loads all variable definitions, which were performed inside a for cycle before the cycle
 * @param vars the variables to be loaded
**/
void store_for_variables(vect_htab_var_t vars);


#endif //__TOKEN_H__
