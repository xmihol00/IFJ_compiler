
#ifndef __ORPN_PRIVATE_H__
#define __ORPN_PRIVATE_H__

//=========================================================================================================
// File:        oRPN_private.h
// Case:        VUT, FIT, IFJ, compiler project
// Date:        9. 12. 2020
// Author(s):   David Mihola
// Contac(s):   xmihol00@stud.fit.vutbr.cz
// Compiled:    gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)
// Description: Header file containing private constant and function declarations for the oRPN module.
//==========================================================================================================

#define STACK_START_SIZE 8L
#define STACK_START_POS -1L
#define ORPN_DATA_START_SIZE 8L
#define ORPN_DATA_START_POS 0

#include "oRPN.h"

#define IDENTIFIER 1
#define FINISH 0xcf
#define SPECIFIC_OR 0xaf
#define NUM 0
#define OP_CODE_SHIFT 5


/**
 * @enum the priority of operators in precedence table.
 *       Each operator is encoded on the 3 MSBs with it's priority by the scanner,
 *       the priority can be than extracted with right shift of 5 bits (see OP_CODE_SHIFT).
 *       The result of the right shift of the operator can be then used as an index to an array
 *       of functions. Each function in the array computes the operator with given priority.
 **/ 
enum operator_types
{
    L1_OP = 0, /// unary +, -, !
    L2_OP = 1, /// *, /
    L3_OP = 2, /// +, -
    L4_OP = 3, /// >, < , >=, <=, !=, ==
    L5_OP = 4, /// &&
    L6_OP = 5, /// ||
    L7_OP = 6, /// = += -= *= /= ,

    // (, ) brackets are added separately
};

/**
 * @struct the representation of the operand
**/
typedef struct operand
{
    uint8_t type;
    token_data_t data;
}operand_t;

/**
 * @struct the representation of the variable memory
**/
typedef struct stack
{
    int64_t pos;
    int64_t size;
    operand_t operands[];
}*stack__t;

/**
 * @struct the representation of the current calculation unit
**/
typedef struct
{
    bool l1_vexpr;          /// true when there is currently calculated partial unknown value in L1 memory
    bool l3_vexpr;          /// true when there is currently calculated partial unknown value in L3 memory
    bool l1_vin;            /// true when unknown value occured in L1 memory
    bool l2_vin;            /// true when unknown value occured in L2 memory
    bool l3_vin;            /// true when unknown value occured in L3 memory
    bool l1_frst_num;       /// true when first value in L1 memory is known constant
    bool relation;          /// true when relational operator is active
    bool vnum_first;        /// true when fisrt operand of partial unknown values is known constant
    uint8_t last_addition;  /// the type of the last added operann (NUM, IDF)
    uint8_t dominant_op;    /// the dominant L2 operator in partial unknown value
    uint8_t func_end_op;    /// operator which follows the end of function
    int8_t num_pos;         /// the position of constant in nums (memory of constants)
    int64_t l1_npos;        /// position of a last known constant in L1 memory
    int64_t l3_npos;        /// position of a last known constant in L3 memory
    vect_uint8_t operators; /// the stack of operators - based on the stack the precedence rules are applied
    operand_t nums[4];      /// stakc of constants
    stack__t l1_mem;        /// the L1 memory - used for calculation of L1, L2, L3 operators, l1_mem->operands[0] is an acumulator for constans
    stack__t l2_mem;        /// the L2 memory - used for calculation of L4 operators
    stack__t l3_mem;        /// the L3 memory - used for calculation of L5, L6 operators, l3_mem->operands[0] is an acumulator for constans
}expr_data_t;

/**
 * @struct the representation of the used calculation units, each new open bracket or function 
 *         creates a new calculation unit, which is then closed after close bracket.
**/
typedef struct
{
    int size;              /// the size of the calculation units stack
    int pos;               /// the position in the calculation units stack
    int allocated;         /// the number of allocated calculation units
    expr_data_t data[];    /// the calculation units
}orpn_stack_t;

void data_const();

void data_dest();

/**
 * @brief function called when two operators are added in succession to the oRPN
 * @param unray the operator which might be unary 
 **/ 
void add_unary(uint8_t unary);

/**
 * @brief aplies the rules of L1 operators i.e unary +, -, !
 **/ 
void l1_op();

/**
 * @brief aplies the rules of L2 operators i.e *, -
 **/ 
void l2_op();

/**
 * @brief aplies the rules of L3 operators i.e +, -
 **/
void l3_op();

/**
 * @brief aplies the rules of L4 operators i.e ==, !=, >=, <=, >, <
 **/
void l4_op();

/**
 * @brief aplies the rules of L5 operator i.e &&
 **/
void l5_op();

/**
 * @brief aplies the rules of L6 operator i.e ||
 **/
void l6_op();

/**
 * @brief aplies the rules of L7 operator i.e :=, =, +=, -=, *=, /=, ',', COND_EXPR_END
 **/
void l7_op();

/**
 * @brief moves data between the memories based on the newly incomming operator
 * @param new_op the currenly added operator
 **/
void move_data(uint8_t new_op);

/**
 * @brief resets the state of a calculation unit
 **/
void reset_calculation();

void l_err_op();

/**
 * @brief loads the result of the calculation to the token list
 **/
void load_to_list();

#endif //__ORPN_PRIVATE_H__