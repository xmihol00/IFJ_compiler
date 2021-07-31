
#ifndef __SCANNER_PRIVATE_H__
#define __SCANNER_PRIVATE_H__

//=========================================================================================================
// File:        scanner_private.h
// Case:        VUT, FIT, IFJ, compiler project
// Date:        9. 12. 2020
// Author(s):   Lukas Foltyn, Vladislav Sokolovskii
// Contac(s):   xfolty17@stud.fit.vutbr.cz, xsokol15@stud.fit.vutbr.cz
// Compiled:    gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)
// Description: Header file containing declaration of functions private for the scanner module.
//==========================================================================================================

#include "scanner.h"

/**
 * @brief Processes a next part of the source code, determines the type of the next lexem. There are four possible 
 *        outcomes: keyword, type (int, string, float64), identifier, function identifier
 * @param output_data The proccesed lexem, in case of keyword or type just the representing token
 * @param firs The first character of the lexem, which is processed
 * @return The data type saved in the output_data
**/
uint8_t keyword_idf(string_t *output_data, char first);

/**
 * @brief Processes a string representation of a number (removes _ and checks if the form is correct). 
 *        Saves the number in string form to the output_data and returns T_INT token,
 *        T_FLOAT token type in case of a decimal number with a floating point.
 *        The parser will then perform the conversion to actual number representation.
 * @param output_data the string where the value of the lexeme will be stored
 * @param first the first character of the number, which is processed
**/
void number(string_t *output_data, int first);

/**
 * @brief Converts a string literal bounded with double quotes to the correct form,
 *  stores the string to the out_put data,
 *  converts #, /, space and ascii characters with the value <= 32 to the escape sequence,
 *  calls auxiliary functions to retrieve the symbols written the coded form e.g. '\n', '\xhh',
 *  after the string was processed scanner returns T_STRING token type
 *
**/
void convert_string(string_t *output_data);

/**
 * @brief Retrieves special character starting with '\' to ascii, calls hex_to_ascii function
 * in case that the next char after '\' is 'x' i.e. character is written in the hexadecimal form
 * @return value of the decoded ASCII character
**/
int retrieve_coded_symbol();

/**
 * @brief Converts hex value starting with "\x" to ASCII
 * @return Decimal value of the character represented as a sequence of chars after "\x"
**/
int hex_to_ascii();

/**
 * @brief Removes comments from the source code,
 * in line comments does not count new line character as the part of the comment, however
 * in block comments EOL is considered as a part of the comment
**/
void skip_comment();

#endif // __SCANNER_PRIVATE_H__
