
#ifndef __VECTOR_PRIVATE_H__
#define __VECTOR_PRIVATE_H__

//=========================================================================================================
// File:        vector_private.h
// Case:        VUT, FIT, IFJ, compiler project
// Date:        9. 12. 2020
// Author(s):   David Mihola
// Contac(s):   xmihol00@stud.fit.vutbr.cz
// Compiled:    gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)
// Description: Header file containing private constant and data type declarations for the vector module.
//==========================================================================================================

#include "vector.h"

#define VECTOR_START_SIZE 8LL
#define VECTOR_START_POS -1LL /// The starting position is -1, vectors use signed int, the reason is a better read options

#define STRING_START_SIZE 64LL
#define STRING_START_POS -1LL
#define STRING_SAFETY_OFFSET 2LL

/**
 * @struct unsigned uint8_t vector
**/
typedef struct vect_uint8
{
    int64_t size;         /// the current size of the allocated memory for the vector
    int64_t pos;          /// the position of the currently last value
    uint8_t values[];     /// dynamically allocated array of unsigned char values
}T_vect_uint8;

/**
 * @struct unsigned int32_t vector
**/
typedef struct vect_int32
{
    int64_t size;          /// the current size of the allocated memory for the vector
    int64_t pos;           /// the position of the currently last value
    int32_t values[];     /// dynamically allocated array of unsigned short values
}T_vect_int32;

/**
 * @struct string vector
**/
typedef struct vect_string
{
    int64_t size;          /// the current size of the allocated memory for the vector
    int64_t pos;           /// the position of the currently last value
    string_t values[];   /// dynamically allocated array of strings values
}T_vect_string;

/**
 * @struct hash table variables vector
**/
typedef struct vect_var
{
    int64_t size;          /// the current size of the allocated memory for the vector
    int64_t pos;           /// the position of the currently last value
    htab_var_t values[];   /// dynamically allocated array of strings values
}T_vect_var;

/**
 * @struct hash table function vector
**/
typedef struct vect_func
{
    int64_t size;          /// the current size of the allocated memory for the vector
    int64_t pos;           /// the position of the currently last value
    htab_func_t values[];   /// dynamically allocated array of hash table function values
}T_vect_func;

/**
 * @struct string (Basically a character vector, but with a zero terminating character.)
**/
typedef struct string
{
    int64_t size;     /// the current size of the allocated area for the string
    int64_t pos;      /// the current position of the last character of the string
    char str[];       /// the array of characters, in which the string is stored
}T_string;

/**
 * @struct vector of linked list couples (two nodes)
**/ 
typedef struct vect_ll_couple
{
    int64_t size;              /// the current size of the allocated area for the vector
    int64_t pos;               /// the current position of the last element of the vector
    token_couple_t values[];   /// the array of characters, in which the string is stored
}T_vect_ll_couple;

/**
 * @struct vector of link list nodes
**/
typedef struct vect_token
{
    int64_t size;       /// the current size of the allocated area for the vector
    int64_t pos;        /// the current position of the last element of the vector
    token_t values[];   /// the array of characters, in which the string is stored
}T_vect_token;

/**
 * @struct vector of vectors of functions
**/
typedef struct double_vect_func
{
    int64_t reader;             /// current position of the next read value
    int64_t size;               /// the current size of the allocated memory for the vector
    int64_t pos;                /// the position of the currently last value
    vect_htab_func_t values[];  /// dynamically allocated array of hash table function values
}T_dvect_func;

/**
 * @struct vector of variable values
**/
typedef struct vect_var_value
{
    int64_t size;               /// the current size of the allocated memory for the vector
    int64_t pos;                /// the position of the currently last value
    var_values_t values[];  /// dynamically allocated array of hash table function values
}T_vect_vval;

#endif
