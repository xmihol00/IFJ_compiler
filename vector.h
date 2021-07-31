
#ifndef __VECTOR_H__
#define __VECTOR_H__

//=========================================================================================================
// File:        vector.h
// Case:        VUT, FIT, IFJ, compiler project
// Date:        9. 12. 2020
// Author(s):   David Mihola
// Contac(s):   xmihol00@stud.fit.vutbr.cz
// Compiled:    gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)
// Description: Header file containing declaration of functions and data types of auxiliary data structures.
//==========================================================================================================

typedef struct vect_uint8 * vect_uint8_t;
typedef struct vect_int32 * vect_int32_t;
typedef struct vect_string * vect_string_t;
typedef struct vect_var * vect_htab_var_t;
typedef struct vect_func * vect_htab_func_t;
typedef struct string * string_t;
typedef struct vect_ll_couple * vect_ll_couple_t;
typedef struct vect_token * vect_token_t;
typedef struct double_vect_func * vect_dbl_func_t;
typedef struct vect_var_value * vect_var_val_t;


#include "enums.h"
#include "symtable.h"
#include "memory.h"
#include "token.h"


// ======================================== uint8_vector START ========================================

/**
 * @brief Creates a new uint8 vector and saves it to the global memory managemet unit
 * @return Pointer to the newly created vector 
**/
vect_uint8_t vect_uint8_const();

/**
 * @brief Returns the current possition of a vector
 * @param vector The vector which position needs to be returned
 * @return The position
**/
int64_t vu8_pos(vect_uint8_t vector);

/**
 * @brief Sets a new value to a specific position in the vector
 * @param vector The vector which position is set
 * @param The set position
 * @param value The value, which is set to a position pos
 * @pre The vector must have at least pos values
**/
void vu8_set_pos(vect_uint8_t vector, int64_t pos, uint8_t value);

/**
 * @brief Adds a new value at the end of a vector
 * @param vector A pointer to the vector to which the new vale is added
 * @param value The new value
**/
void vu8_push(vect_uint8_t *vector, uint8_t value);

/**
 * @brief Adds a new value before the last value in the vector
 * @param vector A pointer to the vector to which the new vale is added
 * @param value The new value
 * @pre The vector has to have at least one value
**/
void vu8_push_under(vect_uint8_t *vector, uint8_t value);

/**
 * @brief Pops multiple values from a vector at once
 * @param vector The popped vector
 * @param count the number of popped values
 * @pre The vector must have at least count values
**/
void vu8_multi_pop(vect_uint8_t vector, int64_t count);

/**
 * @brief Returns a value from a vector on position pos
 * @param vector The vector form which the vale is returned
 * @param pos The index of the retunred value
 * @pre The vector must have at least pos + 1 values
**/
uint8_t vu8_get_pos(vect_uint8_t vector, int64_t pos);

/**
 * @brief Pops multiple values from a vector at under the last value
 * @param vector The popped vector
 * @param count the number of popped values
 * @pre The vector must have at least count + 1 values
**/
void vu8_multi_pop_under(vect_uint8_t vector, int64_t count);

/**
 * @brief Reads a value from the end of a given vector
 * @pre The vector must have at least one stored value
 * @param vector The vector from which the value will be read
 * @return The last value of the given vector
**/
uint8_t vu8_get_last(vect_uint8_t vector);

/**
 * @brief Sets a value of an element at the end of a vector
 * @pre The vector must have at least one stored value
 * @param vector The vector to which the value will be stored
 * @param value The value to be stored
**/
void vu8_set_last(vect_uint8_t vector, uint8_t value);

/**
 * @brief Sets a value of an element offseted from the end of a vector
 * @pre The vector must have at offset stored value
 * @param vector The vector to which the value will be stored
 * @param offset The offset prom the end ofthe vector, where the value will be stored
 * @param value The value to be stored
**/
void vu8_set_offset(vect_uint8_t vector, int64_t offset, uint8_t value);

/**
 * @brief checks for empty vector
 * @param vector the actual vector that is being checked
 * @return bool - true if vector is empty otherwise false is returned
**/ 
bool vu8_is_empty(vect_uint8_t vector);

/**
 * @brief Reads a value offseted value from the end of a given vector
 * @pre The vector must have at least offset values stored
 * @param vector The vector from which the value will be read
 * @param offset The offset from the end of the vector
 * @return The offseted value of the given vector
**/
uint8_t vu8_get_offset(vect_uint8_t vector, int64_t offset);

/**
 * @brief Deletes one value from the end of a given vector
 * @pre The vector must have at least one stored value
 * @param vector The vector, which is popped
 * @return The popped value
**/
uint8_t vu8_pop(vect_uint8_t vector);

/**
 * @brief Deletes all values from a given vector
 * @param vector The vector which is reseted
**/
void vu8_reset(vect_uint8_t vector);

/**
 * @brief Discards (frees) the memory used by a vector, the vector is set to point to NULL
 * @param vector The vector to be discarded
**/
void discard_uint8_vector(vect_uint8_t *vector);

// ========================================= uint8_vector END =========================================

// ======================================== uint32_vector START ========================================

/**
 * @brief Creates a new uint8 vector and saves it to the global memory managemet unit
 * @return Pointer to the newly created vector 
**/
vect_int32_t vect_int32_const();

/**
 * @brief Adds a new value at the end of a vector
 * @param vector A pointer to the vector to which the new vale is added
 * @param value The new value
**/
void vi32_push(vect_int32_t *vector, int32_t value);

/**
 * @brief Reads a value from the end of a given vector
 * @pre The vector must have at least one stored value
 * @param vector The vector from which the value will be read
 * @return The last value of the given vector
**/
int32_t vi32_get_last(vect_int32_t vector);

/**
 * @brief Reads a value offseted value from the end of a given vector
 * @pre The vector must have at least offset values stored
 * @param vector The vector from which the value will be read
 * @param offset The offset from the end of the vector
 * @return The offseted value of the given vector
**/
int32_t vi32_get_offset(vect_int32_t vector, int64_t offset);

/**
 * @brief Deletes one value from the end of a given vector
 * @pre The vector must have at least one stored value
 * @param vector The vector, which is popped
 * @return The popped value
**/
int32_t vi32_pop(vect_int32_t vector);

/**
 * @brief Deletes all values from a given vector
 * @param vector The vector which is reseted
**/
void vect_int32_reset(vect_int32_t vector);

/**
 * @brief Discards (frees) the memory used by a vector, the vector is set to point to NULL
 * @param vector The vector to be discarded
**/
void discard_int32_vector(vect_int32_t *vector);

// ========================================= uint32_vector END =========================================

// ======================================= htab_var_vector START ========================================

/**
 * @brief Creates a new variable vector and saves it to the global memory managemet unit
 * @return Pointer to the newly created vector 
**/
vect_htab_var_t vect_htab_var_const();

/**
 * @brief Adds a new value at the end of a vector
 * @param vector A pointer to the vector to which the new vale is added
 * @param value The new value
**/
void vhtv_push(vect_htab_var_t *vector, htab_var_t value);

/**
 * @brief Reads a value from the end of a given vector
 * @pre The vector must have at least one stored value
 * @param vector The vector from which the value will be read
 * @return The last value of the given vector
**/
htab_var_t vhtv_get_last(vect_htab_var_t vector);

/**
 * @brief Reads an offseted value from the end of a given vector
 * @pre The vector must have at least offset values stored
 * @param vector The vector from which the value will be read
 * @param offset The offset from the end of the vector
 * @return The offseted value of the given vector
**/
htab_var_t vhtv_get_offset(vect_htab_var_t vector, int64_t offset);

/**
 * @brief Reads a value at position position from a given vector
 * @pre The vector must have at least position + 1 values stored
 * @param vector The vector from which the value will be read
 * @param offset The position on which the value is read
 * @return The value on the position of the given vector
**/
htab_var_t vhtv_get_pos(vect_htab_var_t vector, int64_t position);

/**
 * @param vector The vector which the position of last element is needed
 * @return The position of the last element in the vector
**/
int64_t vhtv_pos(vect_htab_var_t vector);

/**
 * @brief Deacreases the size of a given vector by one, the value lost by decreasing is not deallocated
 * @pre The vector must have at least one stored value
 * @param vector The vector, which is popped
 * @return The popped value
**/
htab_var_t vhtv_pop(vect_htab_var_t vector);

/**
 * @brief Loads all the variables stored in the vector to the list in the from the last added, to the first added,
 *        adds the T_ASSGN type befor each loaded variable. The vector is reseted
 * @param vector The vector from which the variables are loaded.
**/
void vhtv_load_to_list_pop(vect_htab_var_t vector);

/**
 * @brief Loads all the variables stored in the vector to the list from the last added, to the first added,
 *        adds the T_ASSGN type befor each loaded variable. The vector remains unchanged
 * @param vector The vector from which the variables are loaded.
**/
void vhtv_load_to_list(vect_htab_var_t vector);

/**
 * @brief Deletes (deallocates) the last value from a given vector, the deleted value must have been previously added to the MMU
 * @pre The vector must have at least one stored value
 * @param vector The vector, which is popped
**/
void vhtv_pop_free(vect_htab_var_t vector);

/**
 * @brief Cleares a given vector, all values left in the vector are not deallocated and lost. All values left in the vector are
 *        not deallocated and lost
 * @param vector The vector which is reseted
**/
void vhtv_reset(vect_htab_var_t vector);

/**
 * @brief Deletes all values from a given vector, all the deleted values must have been previously added to the MMU
 * @param vector The vector which is reseted
**/
void vhtv_reset_free(vect_htab_var_t vector);

/**
 * @brief Discards (frees) the memory used by a given vector, the vector is set to point to NULL
 * @param vector The vector to be discarded
**/
void discard_htab_var_vector(vect_htab_var_t *vector);

/**
 * @brief Discards (frees) the memory used by a given vector and by all values left in that vector, 
 *        the vector is set to point to NULL. The values left in the vector must have been previously added to the MMU.
 * @param vector The vector to be discarded
**/
void discard_htab_var_vector_free(vect_htab_var_t *vector);

/**
 * @brief checks for empty vector
 * @param vector the actual vector that is being checked
 * @return bool - true if vector is empty otherwise false is returned
**/
bool vhtv_is_empty(vect_htab_var_t vector);

// ======================================== htab_var_vector END =========================================

// ======================================== string_vector START =========================================

/**
 * @brief Creates a new vector of strings and saves it to the global memory managemet unit
 * @return Pointer to the newly created vector
**/
vect_string_t vect_str_const();

/**
 * @brief Adds a new value at the end of a vector
 * @param vector A pointer to the vector to which the new vale is added
 * @param value The new value
**/
void vstr_push(vect_string_t *vector, string_t value);

/**
 * @brief Reads a value from the end of a given vector
 * @pre The vector must have at least one stored value
 * @param vector The vector from which the value will be read
 * @return The last value of the given vector
**/
string_t vstr_get_last(vect_string_t vector);

/**
 * @brief Reads a value offseted value from the end of a given vector
 * @pre The vector must have at least offset values stored
 * @param vector The vector from which the value will be read
 * @param offset The offset from the end of the vector
 * @return The offseted value of the given vector
**/
string_t vstr_get_offset(vect_string_t vector, int64_t offset);

/**
 * @brief Deacreases the size of a given vector by one, the value lost by decreasing is not deallocated
 * @pre The vector must have at least one stored value
 * @param vector The vector, which is popped
 * @return The popped value
**/
string_t vstr_pop(vect_string_t vector);

/**
 * @brief Deletes (deallocates) the last value from a given vector, the deleted value must have been previously added to the MMU
 * @pre The vector must have at least one stored value
 * @param vector The vector, which is popped
**/
void vstr_pop_free(vect_string_t vector);

/**
 * @brief Cleares a given vector, all values left in the vector are not deallocated and lost
 * @param vector The vector which is reseted
**/
void vstr_reset(vect_string_t vector);

/**
 * @brief Deletes all values from a given vector, all the deleted values must have been previously added to the MMU
 * @param vector The vector which is reseted
**/
void vstr_reset_free(vect_string_t vector);

/**
 * @brief Discards (frees) the memory used by a vector, the vector is set to point to NULL. All values left in the vector are
 *        not deallocated and lost
 * @param vector The vector to be discarded
**/
void discard_str_vector(vect_string_t *vector);

/**
 * @brief Discards (frees) the memory used by a given vector and by all values left in that vector, 
 *        the vector is set to point to NULL. The values left in the vector must have been previously added to the MMU.
 * @param vector The vector to be discarded
**/
void discard_str_vector_free(vect_string_t *vector);

// ======================================== string_vector END =========================================

// ========================================== string START ============================================

/**
 * @brief Creates a new string and saves it to the global memory managemet unit
 * @return Pointer to the newly created string
**/
string_t string_const();

/**
 * @brief Adds a new character at the end of a string
 * @param str The pointer to the string to which the new vale is added
 * @param value The new value
**/
void string_char_push(string_t *str, char value);

/**
 * @brief Reads the last value from a given string
 * @pre The vector must have at least one stored value
 * @param vector The string from which the value will be read
 * @return The last value in the given string
**/
char string_get_last(string_t str);

/**
 * @brief Reads the last value from a given string and delete it
 * @param vector The string from which the value will be read and deleted
 * @pre The vector must have at least one stored value
 * @return The last value in the given string
**/
char string_char_pop(string_t str);

/**
 * @brief Adds a new substring at the end of a string, the substring must be at least one character long
 * @param str A pointer to the string to which the new vale is added
 * @param substr The new substring to be added
 * @param lenght The lenght of the newly added substring
**/
void string_substr_push(string_t *str, char *substr, int64_t lenght);

/**
 * @brief Reads a given string
 * @param str A pointer to the string, which is read
**/
char *string_read(string_t str);

/**
 * @brief Concatinates two strings, the source string must be at least one character long
 * @param str_dest A pointer to the destiantion string, to which the concatination is executed
 * @param str_src The source string from which the concatination is executed
**/
void string_concat(string_t *str_dest, string_t str_src);

/**
 * @brief Rewrites string with another one, the source string must be at least one character long
 * @param str_dest A pointer to the destiantion string, to which the rewrite is executed
 * @param str_src The source string from which the rewrite is executed
**/
void string_rewrite(string_t *str_dest, string_t str_src);

/**
 * @brief Deletes a given string
 * @param vector The string which is reseted
**/
void string_reset(string_t str);

/**
 * @brief Discards (frees) the memory used by a string, the vector is set to point to NULL
 * @param vector The vector to be discarded
**/
void discard_string(string_t *str);

// =========================================== string END =============================================

// ===================================== htab_func_vector START =======================================

/**
 * @brief Creates a new variable vector and saves it to the global memory managemet unit
 * @return Pointer to the newly created vector 
**/
vect_htab_func_t vect_htab_func_const();

/**
 * @brief Adds a new value at the end of a vector
 * @param vector A pointer to the vector to which the new vale is added
 * @param value The new value
**/
void vfht_push(vect_htab_func_t *vector, htab_func_t value);

/**
 * @brief Adds a new value at the end of a vector, if the value is not already present in the vector
 * @param vector A pointer to the vector to which the new vale is added
 * @param value The new value
**/
void vfht_push_unique(vect_htab_func_t *vector, htab_func_t value);

/**
 * @brief Reads a value from the end of a given vector
 * @pre The vector must have at least one stored value
 * @param vector The vector from which the value will be read
 * @return The last value of the given vector
**/
htab_func_t vfht_get_last(vect_htab_func_t vector);

/**
 * @brief Reads a value offseted value from the end of a given vector
 * @pre The vector must have at least offset values stored
 * @param vector The vector from which the value will be read
 * @param offset The offset from the end of the vector
 * @return The offseted value of the given vector
**/
htab_func_t vfht_get_offset(vect_htab_func_t vector, int64_t offset);

/**
 * @return The position of the last element in a given vector
**/
int64_t vfht_pos(vect_htab_func_t vector);

/**
 * @brief Deacreases the size of a given vector by one, the value lost by decreasing is not deallocated
 * @pre The vector must have at least one stored value
 * @param vector The vector, which is popped
 * @return The popped value
**/
htab_func_t vfht_pop(vect_htab_func_t vector);

/**
 * @brief Cleares a given vector, all values left in the vector are not deallocated and lost. All values left in the vector are
 *        not deallocated and lost
 * @param vector The vector which is reseted
**/
void vfht_reset(vect_htab_func_t vector);

/**
 * @brief Discards (frees) the memory used by a given vector, the vector is set to point to NULL
 * @param vector The vector to be discarded
**/
void discard_htab_func_vector(vect_htab_func_t *vector);

// ======================================= htab_func_vector END =========================================

// ====================================== ll_couple_vector START ========================================

/**
 * @brief Creates a new linked list couple vector and saves it to the global memory managemet unit
 * @return Pointer to the newly created vector 
**/
vect_ll_couple_t vect_ll_couple_const();

/**
 * @brief Adds a first new value at the end of a vector
 * @param vector A pointer to the vector to which the new vale is added
 * @param value The fisrt value of the couple
**/
void vllc_push_fisrt(vect_ll_couple_t *vector, token_t value);

/**
 * @pre   vllc_push_first must be always called fisrt
 * @brief Adds a second new value at the end of a vector
 * @param vector The vector to which the new vale is added
 * @param value The second value of the couple
**/
void vllc_add_second(vect_ll_couple_t vector, token_t value);

/**
 * @brief Pops a couple from the vector and stores it to the given parameters
 * @pre The vector must have at least one stored value
 * @param vector The vector from which the value will be read
 * @param first The fisrt token of the pair
 * @param second The second token of the pair
 * @return The last value of the given vector
**/
void vllc_pop_couple(vect_ll_couple_t vector, token_t *fisrt, token_t *second);

/**
 * @return The position of the last element in a given vector
**/
int64_t vllc_pos(vect_ll_couple_t vector);

/**
 * @brief Gets the second to last element of the vector
 * @pre the vector must have at least two values
 * @param vector the vector from which the value is taken
 * @return The value of the second to last element
**/
token_t vllc_get_last_second(vect_ll_couple_t vector);

/**
 * @brief Discards (frees) the memory used by a vector, the vector is set to point to NULL.
 *        All values left in the vector are lost and not deallocated.
 * @param vector The vector to be discarded
**/
void discard_ll_couple_vector(vect_ll_couple_t *vector);

// ======================================= ll_couple_vector END =========================================

// ======================================== token_vector START ==========================================

/**
 * @brief Creates a new uint8 vector and saves it to the global memory managemet unit
 * @return Pointer to the newly created vector 
**/
vect_token_t vect_token_const();

/**
 * @brief Adds a new value at the end of a vector
 * @param vector A pointer to the vector to which the new vale is added
 * @param value The new value
**/
void vtn_push(vect_token_t *vector, token_t value);

/**
 * @brief Gets the second to last element of the vector
 * @pre the vector must have at least two values
 * @param vector the vector from which the value is taken
 * @return The value of the second to last element
**/
token_t vtn_get_2nd_last(vect_token_t vector);

/**
 * @brief Gets a value from the end of a given vector
 * @pre The vector must have at least one stored value
 * @param vector The vector from which the value will be read
 * @return The last value of the given vector
**/
token_t vtn_get_last(vect_token_t vector);

/**
 * @brief Replaces last two values of a given vector with one new value
 * @pre The vector must have at least two stored value
 * @param vector The vector where the values will be replaces
 * @param token The new value
**/
void vtn_replace_last_2(vect_token_t vector, token_t token);

/**
 * @brief Resets given vector
 * @param vector The vector which is reseted
**/
void vtn_reset(vect_token_t vector);

/**
 * @brief Deletes one value from the end of a given vector
 * @pre The vector must have at least one stored value
 * @param vector The vector, which is popped
 * @return The popped value
**/
token_t vtn_pop(vect_token_t vector);

/**
 * @brief Discards (frees) the memory used by a vector, the vector is set to point to NULL. 
 *        All values left in the vector are lost and not deallocated
 * @param vector The vector to be discarded
**/
void discard_token_vector(vect_token_t *vector);

/**
 * @brief Discards (frees) the memory used by a vector, the vector is set to point to NULL. 
 *        All values stored in the vectro are also deallocated.
 * @param vector The vector to be discarded
**/
void discard_token_vector_free(vect_token_t *vector);

// ========================================= token_vector END ==========================================

// ====================================== double_vect_func START =======================================

/**
 * @brief Creates a new vector of vectors of functions
 * @return Pointer to the newly created vector 
**/
vect_dbl_func_t vect_dbl_func_const();

/**
 * @brief Adds a new value at the end of a vector
 * @param vector A pointer to the vector to which the new vale is added
 * @param value The new value
**/
void vdfht_push(vect_dbl_func_t *vector, vect_htab_func_t value);

/**
 * @brief Reads a value from the end of a given vector
 * @pre The vector must have at least one stored value
 * @param vector The vector from which the value will be read
 * @return The last value of the given vector
**/
vect_htab_func_t vdfht_get_next(vect_dbl_func_t vector);

/**
 * @brief Resets given vector
 * @param vector The vector which is reseted
**/
void reset_dbl_func_vector(vect_dbl_func_t vector);

/**
 * @brief Discards (frees) the memory used by a given vector, the vector is set to point to NULL
 * @param vector The vector to be discarded
**/
void discard_dbl_func_vector(vect_dbl_func_t *vector);

// ======================================= double_vect_func END ========================================

// ======================================== vect_var_val START =========================================

/**
 * @brief Creates a new uint8 vector and saves it to the global memory managemet unit
 * @return Pointer to the newly created vector 
**/
vect_var_val_t vect_var_val_const();

/**
 * @brief Adds a new value at the end of a vector
 * @param vector A pointer to the vector to which the new vale is added
 * @param value The new value
**/
void vvv_push(vect_var_val_t *vector, var_values_t value);

/**
 * @brief Adds a new value at the end of a vector, which is copied from the last value
 * @pre The vector must have at least one stored value
 * @param vector A pointer to the vector to which the new vale is added
**/
void vvv_push_copy(vect_var_val_t *vector);

/**
 * @brief Changes the last value of the vector by copiing the second to last value
**/
void vvv_change_last_copy(vect_var_val_t vector);

/**
 * @brief Compares last two values of the vector
**/
bool vvv_compare_last_two(vect_var_val_t vector, uint8_t type);

/**
 * @brief Reads a value from the end of a given vector
 * @pre The vector must have at least one stored value
 * @param vector The vector from which the value will be read
 * @return The last value of the given vector
**/
var_values_t vvv_get_last(vect_var_val_t vector);

/**
 * @brief Reads a value offseted value from the end of a given vector
 * @pre The vector must have at least offset values stored
 * @param vector The vector from which the value will be read
 * @param offset The offset from the end of the vector
 * @return The offseted value of the given vector
**/
var_values_t vvv_get_offset(vect_var_val_t vector, int64_t offset);

/**
 * @brief changes the last value of the vector
**/
void vvv_change_last(vect_var_val_t vector, var_values_t value);

/**
 * @brief returns the current position in the vecotr
**/
int64_t vvv_pos(vect_var_val_t vector);

/**
 * @brief Deallocates a string from the end of a vector, if there any.
 * @param vector The vector from which the value will be deallocated
**/
void vvv_free_last_string(vect_var_val_t vector);


/**
 * @brief Deallocates all strings and resets the vector
 * @param vector The vector which will be reseted
**/
void vvv_free_reset_strings(vect_var_val_t vector);

/**
 * @brief Deletes one value from the end of a given vector
 * @pre The vector must have at least one stored value
 * @param vector The vector, which is popped
 * @return The popped value
**/
var_values_t vvv_pop(vect_var_val_t vector);

/**
 * @brief Deletes all values from a given vector
 * @param vector The vector which is reseted
**/
void vect_var_val_reset(vect_var_val_t vector);

/**
 * @brief Discards (frees) the memory used by a vector, the vector is set to point to NULL
 * @param vector The vector to be discarded
**/
void discard_var_val_vector(vect_var_val_t *vector);

// ========================================= vect_var_val END ==========================================
#endif
