
#include "oRPN_private.h"

//=========================================================================================================
// File:        oRPN.c
// Case:        VUT, FIT, IFJ, compiler project
// Date:        9. 12. 2020
// Author(s):   David Mihola
// Contac(s):   xmihol00@stud.fit.vutbr.cz
// Compiled:    gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)
// Description: File containing definitions of functions responsible for precedence syntax analysis,
//              full and partial expression optimalization.
//==========================================================================================================

// memory of calculation units
orpn_stack_t *G_data_orpn;

// current used unit for calculation
expr_data_t *G_orpn;

/// array of functions called based on the precedence of the operators
void (*op_lvl_functions[8])() = {l1_op, l2_op, l3_op, l4_op, l5_op, l6_op, l7_op, l_err_op};

/// macros for pushing memory stacks
#define PUSH_L1 if (++G_orpn->l1_mem->pos == G_orpn->l1_mem->size) \
{G_orpn->l1_mem->size *= 2; stack__t tmp = realloc(G_orpn->l1_mem, sizeof(struct stack) + G_orpn->l1_mem->size * sizeof(struct operand));\
if (tmp == NULL) exit(MEM_ERR); G_orpn->l1_mem = tmp;}

#define PUSH_L2 if (++G_orpn->l2_mem->pos == G_orpn->l2_mem->size) \
{G_orpn->l2_mem->size *= 2; stack__t tmp = realloc(G_orpn->l2_mem, sizeof(struct stack) + G_orpn->l2_mem->size * sizeof(struct operand));\
if (tmp == NULL) exit(MEM_ERR); G_orpn->l2_mem = tmp;}

#define PUSH_L3 if (++G_orpn->l3_mem->pos == G_orpn->l3_mem->size) \
{G_orpn->l3_mem->size *= 2; stack__t tmp = realloc(G_orpn->l3_mem, sizeof(struct stack) + G_orpn->l3_mem->size * sizeof(struct operand));\
if (tmp == NULL) exit(MEM_ERR); G_orpn->l3_mem = tmp;}

#define LOG_ERROR(MESSAGE) break;

// the precedence table is not needed, therfore not implemented, see oRPN_private.h

void init_orpn()
{
    G_data_orpn = calloc(1, sizeof(orpn_stack_t) + ORPN_DATA_START_SIZE * sizeof(expr_data_t));
    if (G_data_orpn == NULL)
        exit(MEM_ERR);
    
    G_data_orpn->pos = ORPN_DATA_START_POS;
    G_data_orpn->size = ORPN_DATA_START_SIZE;
    G_data_orpn->allocated = ORPN_DATA_START_POS;

    G_orpn = &G_data_orpn->data[0];
    data_const();
}

void dispose_orpn()
{
    if (G_data_orpn != NULL)
    {
        for (int i = 0; i <= G_data_orpn->allocated; i++)
        {
            if (i <= G_data_orpn->pos)
            {
                for (uint8_t j = 0; j <= G_data_orpn->data[i].num_pos; j++)
                {
                    if (G_data_orpn->data[i].nums[j].type == T_STRING)
                    {
                        free(G_data_orpn->data[i].nums[j].data.string);
                    }
                }

                for (int j = 0; j <= G_data_orpn->data[i].l1_mem->pos; j++)
                {
                    if (G_data_orpn->data[i].l1_mem->operands[j].type == T_STRING)
                    {
                        free(G_data_orpn->data[i].l1_mem->operands[j].data.string);
                    }
                }

                for (int j = 0; j <= G_data_orpn->data[i].l2_mem->pos; j++)
                {
                    if (G_data_orpn->data[i].l2_mem->operands[j].type == T_STRING)
                    {
                        free(G_data_orpn->data[i].l2_mem->operands[j].data.string);
                    }
                }

                for (int j = 0; j <= G_data_orpn->data[i].l3_mem->pos; j++)
                {
                    if (G_data_orpn->data[i].l3_mem->operands[j].type == T_STRING)
                    {
                        free(G_data_orpn->data[i].l3_mem->operands[j].data.string);
                    }
                }
            }

            discard_uint8_vector(&G_data_orpn->data[i].operators);
            free(G_data_orpn->data[i].l1_mem);
            free(G_data_orpn->data[i].l2_mem);
            free(G_data_orpn->data[i].l3_mem);
        }
        free(G_data_orpn);
        G_data_orpn = NULL;
    }
}

void data_const()
{
    G_orpn->operators = vect_uint8_const();
    vu8_push(&G_orpn->operators, EMPTY);
  
    G_orpn->l1_mem = calloc(1, sizeof(struct stack) + STACK_START_SIZE * sizeof(struct operand));
    if (G_orpn->l1_mem == NULL)
        exit(MEM_ERR);
    
    G_orpn->l1_mem->pos = STACK_START_POS + 1;
    G_orpn->l1_mem->size = STACK_START_SIZE;
    G_orpn->l1_mem->operands[0].type = NUM;
    
    G_orpn->l2_mem = calloc(1, sizeof(struct stack) + STACK_START_SIZE * sizeof(struct operand));
    if (G_orpn->l2_mem == NULL)
        exit(MEM_ERR);
    
    G_orpn->l2_mem->pos = STACK_START_POS;
    G_orpn->l2_mem->size = STACK_START_SIZE;

    G_orpn->l3_mem = calloc(1, sizeof(struct stack) + STACK_START_SIZE * sizeof(struct operand));
    if (G_orpn->l3_mem == NULL)
        exit(MEM_ERR);
    
    G_orpn->l3_mem->pos = STACK_START_POS + 1;
    G_orpn->l3_mem->size = STACK_START_SIZE;
    G_orpn->l3_mem->operands[0].type = NUM;

    G_orpn->l1_npos = -1;
    G_orpn->l3_npos = -1;
    G_orpn->l1_mem->operands[0].type = NUM;
    G_orpn->l3_mem->operands[0].type = NUM;
    G_orpn->num_pos = -1;
    G_orpn->dominant_op = EMPTY;
    G_orpn->l1_vexpr = false;
    G_orpn->l3_vexpr = false;
    G_orpn->l1_vin = false;
    G_orpn->l2_vin = false;
    G_orpn->l3_vin = false;
    G_orpn->vnum_first = false;
    G_orpn->relation = false;
    G_orpn->l1_frst_num = false;
    G_orpn->func_end_op = 0;
}

void data_dest()
{
    discard_uint8_vector(&G_orpn->operators);
    free(G_orpn->l1_mem);
    free(G_orpn->l2_mem);
    free(G_orpn->l3_mem);
}

void orpn_add_var(htab_var_t var)
{
    if (vu8_get_last(G_orpn->operators) == IDENTIFIER)
    {
        exit(SYNTAX_ERR); // two successive operators, function calls or constants
    }

    if (is_var_assignable() && has_var_value(var)) // the value of a variable is known and can be used
    {
        switch (get_var_type(var))
        {
            case T_STRING:
                G_orpn->nums[++G_orpn->num_pos].data.string = malloc(strlen(get_var_value(var).string) + 1);
                if (G_orpn->nums[G_orpn->num_pos].data.string == NULL)
                {
                    exit(MEM_ERR);
                }
                strcpy(G_orpn->nums[G_orpn->num_pos].data.string, get_var_value(var).string);
                G_orpn->nums[G_orpn->num_pos].type = T_STRING;
                break;

            case T_FLOAT:
                G_orpn->nums[++G_orpn->num_pos].data.float64 = get_var_value(var).float64;
                G_orpn->nums[G_orpn->num_pos].type = T_FLOAT;
                break;

            case T_INT:
                G_orpn->nums[++G_orpn->num_pos].data.float64 = get_var_value(var).float64;
                G_orpn->nums[G_orpn->num_pos].type = T_INT;
                break;
            
            case T_BOOL:
                G_orpn->nums[++G_orpn->num_pos].data.float64 = get_var_value(var).float64;
                G_orpn->nums[G_orpn->num_pos].type = T_BOOL;
                break;
        }

        G_orpn->last_addition = NUM;
        vu8_push(&G_orpn->operators, IDENTIFIER);
        return;
    }
    // the value of the variable is unknown and last added operator was of level 2 
    // and there is not ongoing subexpression including unknown values
    else if ((vu8_get_last(G_orpn->operators) >> OP_CODE_SHIFT) == L2_OP && !G_orpn->l1_vexpr)
    {
        PUSH_L1
        G_orpn->l1_mem->operands[G_orpn->l1_mem->pos] = G_orpn->nums[G_orpn->num_pos];
        G_orpn->l1_npos = G_orpn->l1_mem->pos;
        G_orpn->num_pos--;

        PUSH_L1
        G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].data.var = var;
        G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].type = VAR;

        PUSH_L1
        G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].data.op = vu8_pop(G_orpn->operators);
        G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].type = T_OP;
        G_orpn->dominant_op = G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].data.op;
        G_orpn->vnum_first = true;

    }
    // the value of the variable is unknown and last added operator was of level 1 and the second to last of level 2
    // and there is not ongoing subexpression including unknown values
    else if (vu8_get_last(G_orpn->operators) >> OP_CODE_SHIFT == L1_OP && vu8_pos(G_orpn->operators) > 0 
        && (vu8_get_offset(G_orpn->operators, 1) >> OP_CODE_SHIFT) == L2_OP && !G_orpn->l1_vexpr)
    {
        PUSH_L1
        G_orpn->l1_mem->operands[G_orpn->l1_mem->pos] = G_orpn->nums[G_orpn->num_pos];
        G_orpn->l1_npos = G_orpn->l1_mem->pos;
        G_orpn->num_pos--;

        PUSH_L1
        G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].data.var = var;
        G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].type = VAR;

        PUSH_L1
        G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].data.op = vu8_pop(G_orpn->operators);
        G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].type = T_OP;

        PUSH_L1
        G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].data.op = vu8_pop(G_orpn->operators);
        G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].type = T_OP;
        G_orpn->dominant_op = G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].data.op;
        G_orpn->vnum_first = true;
    }
    else // all other cases
    {
        if (vu8_get_last(G_orpn->operators) == DIV || vu8_get_last(G_orpn->operators) != G_orpn->dominant_op)
        {
            G_orpn->vnum_first = false;
            G_orpn->l1_npos = -1;
        }

        PUSH_L1
        G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].data.var = var;
        G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].type = VAR;
    }

    vu8_push(&G_orpn->operators, IDENTIFIER); //storing the identifier occurence for the syntax analysis
    G_orpn->l1_vin = G_orpn->l1_vexpr = true;
    G_orpn->last_addition = VAR;
}

void orpn_add_func(htab_func_t func)
{
    if (vu8_get_last(G_orpn->operators) == IDENTIFIER)
    {
        exit(SYNTAX_ERR); // two successive operators, function calls or constants
    }

    // last added operator was of level 2 and there is not ongoing subexpression including unknown values
    if ((vu8_get_last(G_orpn->operators) >> OP_CODE_SHIFT) == L2_OP && !G_orpn->l1_vexpr)
    {
        G_orpn->dominant_op = vu8_get_last(G_orpn->operators);

        PUSH_L1
        G_orpn->l1_mem->operands[G_orpn->l1_mem->pos] = G_orpn->nums[G_orpn->num_pos];
        G_orpn->l1_npos = G_orpn->l1_mem->pos;
        G_orpn->num_pos--;

        PUSH_L1
        G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].type = T_FUNC_CALL;

        PUSH_L1
        G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].data.func = func;
        G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].type = FUNC;

        G_orpn->func_end_op = vu8_pop(G_orpn->operators);
        G_orpn->vnum_first = true;
    }
    // last added operator was of level 1, second to last of level 2 
    // and there is not ongoing subexpression including unknown values
    else if (vu8_pos(G_orpn->operators) > 0 && vu8_get_offset(G_orpn->operators, 1) >> OP_CODE_SHIFT == L2_OP 
        && vu8_get_last(G_orpn->operators) == NEG && !G_orpn->l1_vexpr)
    {
        vu8_pop(G_orpn->operators);
        G_orpn->dominant_op = vu8_get_last(G_orpn->operators);

        PUSH_L1
        G_orpn->l1_mem->operands[G_orpn->l1_mem->pos] = G_orpn->nums[G_orpn->num_pos];
        G_orpn->l1_npos = G_orpn->l1_mem->pos;
        G_orpn->num_pos--;
        
        PUSH_L1
        G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].type = T_OP;
        G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].data.op = ZERO;
       
        G_orpn->func_end_op = vu8_get_last(G_orpn->operators);
        vu8_set_last(G_orpn->operators, NEG);

        PUSH_L1
        G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].type = T_FUNC_CALL;

        PUSH_L1
        G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].data.func = func;
        G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].type = FUNC;
        G_orpn->vnum_first = true;
    }
    else // all other cases
    {
        if (vu8_get_last(G_orpn->operators) == DIV)
        {
            G_orpn->vnum_first = false;
            G_orpn->l1_npos = -1;
        }
        
        G_orpn->func_end_op = 1;

        if (vu8_get_last(G_orpn->operators) == NEG)
        {
            PUSH_L1
            G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].type = T_OP;
            G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].data.op = ZERO;
        }

        PUSH_L1
        G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].type = T_FUNC_CALL;

        PUSH_L1
        G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].data.func = func;
        G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].type = FUNC;

    }

    PUSH_L1
    G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].type = T_PARAM;

    G_orpn->l1_vin = G_orpn->l1_vexpr = true;
    G_orpn->last_addition = VAR;
    orpn_add_ob(); // function identifier includes openining bracket
}

void orpn_add_value(uint8_t type, token_data_t data)
{
    if (vu8_get_last(G_orpn->operators) == IDENTIFIER)
    {
        exit(SYNTAX_ERR); // two successive operators, function calls or constants
    }

    if (type == T_STRING)
    {
        G_orpn->nums[++G_orpn->num_pos].data.string = malloc(strlen(data.string) + 1);
        if (G_orpn->nums[G_orpn->num_pos].data.string == NULL)
        {
            exit(MEM_ERR);
        }
        strcpy(G_orpn->nums[G_orpn->num_pos].data.string, data.string);
    }
    else
    {
        // the array has fixed size, no push needed
        G_orpn->nums[++G_orpn->num_pos].data = data;
    }

    G_orpn->nums[G_orpn->num_pos].type = type;
    G_orpn->last_addition = NUM; // constant as the last added operand

    vu8_push(&G_orpn->operators, IDENTIFIER); // syntax analysis
}

void orpn_add_operator(uint8_t operator)
{
    if (vu8_get_last(G_orpn->operators) == IDENTIFIER) // last added value to the oRPN should be IDF (function or variable)
    {
        vu8_pop(G_orpn->operators);
    }
    // if there are no values returned in function return
    else if (operator == RETURN_END && vu8_get_last(G_orpn->operators) == EMPTY)
    {
        if (G_data_orpn->pos > 0)
        {
            exit(SYNTAX_ERR); // wrong count of brackets
        }
    }
    // when the added operator is first or another operator is on top of the stack
    else if (vu8_get_last(G_orpn->operators) == EMPTY || vu8_get_last(G_orpn->operators) >= NEG)
    {
        add_unary(operator);
        return; // no operators are evaluated, as unary operators have the lowest priority and are RIGHT asociative
    }

    // all operators are left asociative, therfore are operators which have the same or lower priority a evaluated
    while (operator >> OP_CODE_SHIFT >= vu8_get_last(G_orpn->operators) >> OP_CODE_SHIFT)
    {
        // indexing appropriate function, where the operator is evaluated and popped from the stack
        op_lvl_functions[vu8_get_last(G_orpn->operators) >> OP_CODE_SHIFT]();
    }    

    // storing the incoming operator
    vu8_push(&G_orpn->operators, operator);
    // moving data to higher memory level, if neseccary
    move_data(operator);
}

void add_unary(uint8_t unary)
{
    switch (unary)
    {
        case ADD: //unary + is firstly represented as ADD, then changed to POS if necessary
            switch (vu8_get_last(G_orpn->operators))
            {
                case NOT:
                case POS_NOT:
                    //exit(SYNTAX_ERR); // combination of negation and unary +
                    exit(SEMANTIC_EXPR_ERR); // Go uses semantic err..
                
                case ADD:
                case MUL:
                case DIV:
                case SUB:
                case NEG:
                case POS:
                    break;
                
                default:
                    vu8_push(&G_orpn->operators, POS);
            }
            break;
        
        case SUB: // unary - is firstly represented as SUB, then changed to NEG if necessary
            switch (vu8_get_last(G_orpn->operators))
            {
                case NOT:
                case POS_NOT:
                    //exit(SYNTAX_ERR); // combination of negation and unary -
                    exit(SEMANTIC_EXPR_ERR); // Go uses semantic err..
                
                case ADD:
                    vu8_set_last(G_orpn->operators, SUB); // nagating ADD
                    break;

                case POS:
                    vu8_set_last(G_orpn->operators, NEG); // changing POS to NEG
                    break;
                
                case SUB:
                    vu8_set_last(G_orpn->operators, ADD); // negating SUB
                    break;
                
                case NEG:
                    vu8_set_last(G_orpn->operators, POS); // changing NEG to POS
                    break;
                
                default:
                    vu8_push(&G_orpn->operators, NEG); // creating NEG
                    break;

            }
            break;
        
        case NOT:
            switch (vu8_get_last(G_orpn->operators))
            {
                case ADD:
                case MUL:
                case SUB:
                case DIV:
                    //exit(SYNTAX_ERR); // negation in combination with other unary operators
                    exit(SEMANTIC_EXPR_ERR); // Go uses semantic err..
                
                case NOT:
                    vu8_set_last(G_orpn->operators, POS_NOT); // NOT -> POS_NOT (not generated just for the semantic control)
                    break;
                
                case POS_NOT:
                    vu8_set_last(G_orpn->operators, NOT); // POS_NOT -> NOT
                    break;

                default:
                    vu8_push(&G_orpn->operators, unary); // creating NOT
                    break;
            }
            break;
            
        default:
            exit(SYNTAX_ERR); // wrong unary operator i.e *, /
    }
}

void l1_op() // evaluation of L1 operator
{
    if (G_orpn->last_addition == VAR)
    {
        // addition of the operator to the L1 memory, when it refers to a variable or function
        PUSH_L1
        switch(vu8_pop(G_orpn->operators))
        {
            case NOT:
                G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].data.op = NOT;
                G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].type = T_OP;
                break;
            
            case NEG:
                G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].data.op = NEG;
                G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].type = T_OP;
                break;
            
            case POS:
                G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].data.op = POS;
                G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].type = T_OP;
                break;
            
            case POS_NOT:
                G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].data.op = POS_NOT;
                G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].type = T_OP;
                break;
        }
    }
    else
    {
        // static calculation with constant
        switch(vu8_pop(G_orpn->operators))
        {
            case NOT:
                if (G_orpn->nums[G_orpn->num_pos].type != T_BOOL)
                {
                    exit(SEMANTIC_EXPR_ERR);
                }
                G_orpn->nums[G_orpn->num_pos].data.boolean = !G_orpn->nums[G_orpn->num_pos].data.boolean;
                break;
            
            case NEG:
                if (G_orpn->nums[G_orpn->num_pos].type == T_FLOAT)
                {
                    G_orpn->nums[G_orpn->num_pos].data.float64 = -G_orpn->nums[G_orpn->num_pos].data.float64;
                }
                else if (G_orpn->nums[G_orpn->num_pos].type == T_INT)
                {
                    G_orpn->nums[G_orpn->num_pos].data.integer = -G_orpn->nums[G_orpn->num_pos].data.integer;
                }
                else
                {
                    exit(SEMANTIC_EXPR_ERR);
                }
                break;
            
            case POS:
                break;
        }
    }
}

void l2_op() // evaluation of L2 operator
{
    if (G_orpn->l1_vexpr) // unknown value in L1 memory
    {   
        if (G_orpn->last_addition == NUM) // last added operand is a consant
        {
            // conditions for partial reduction of constant in uknown value
            if (G_orpn->l1_npos >= 0 && G_orpn->dominant_op == vu8_get_last(G_orpn->operators) &&
                G_orpn->l1_mem->operands[G_orpn->l1_npos].type == T_INT && G_orpn->nums[G_orpn->num_pos].type == T_INT)
            {
                // static calculation
                switch(vu8_pop(G_orpn->operators))
                {
                    case MUL:
                        G_orpn->l1_mem->operands[G_orpn->l1_npos].data.integer *= G_orpn->nums[G_orpn->num_pos].data.integer;
                        break;

                    case DIV:
                        if (G_orpn->nums[G_orpn->num_pos].data.integer == 0)
                        {
                            exit(SEMANTIC_ZERO_DIV_ERR);
                        }
                        if (G_orpn->vnum_first) // reducing the first constant in an uknwon value by dividing it
                        {
                            G_orpn->l1_mem->operands[G_orpn->l1_npos].data.integer /= G_orpn->nums[G_orpn->num_pos].data.integer;
                        }
                        else // reduction in multiple successive divisions results in multiplying the dividor
                        {
                            G_orpn->l1_mem->operands[G_orpn->l1_npos].data.integer *= G_orpn->nums[G_orpn->num_pos].data.integer;
                        }
                        break;
                }
            }
            else if (G_orpn->l1_npos >= 0 && 
                     G_orpn->l1_mem->operands[G_orpn->l1_npos].type == T_FLOAT && G_orpn->nums[G_orpn->num_pos].type == T_FLOAT)
            {
                // same as integer
                switch(vu8_pop(G_orpn->operators))
                {
                    case MUL:
                        if (G_orpn->dominant_op == MUL || G_orpn->vnum_first)
                        {
                            G_orpn->l1_mem->operands[G_orpn->l1_npos].data.float64 *= G_orpn->nums[G_orpn->num_pos].data.float64;
                        }
                        else
                        {
                            // if the dominant op is the same, the calculation must be reversed
                            G_orpn->l1_mem->operands[G_orpn->l1_npos].data.float64 /= G_orpn->nums[G_orpn->num_pos].data.float64;
                        }
                        break;

                    case DIV:
                        if (G_orpn->nums[0].data.float64 == 0.0) // based on IEEE 754 standard it is save to compare with 0.0
                        {
                            exit(SEMANTIC_ZERO_DIV_ERR);
                        }
                        if (G_orpn->dominant_op == MUL || G_orpn->vnum_first)
                        {
                            G_orpn->l1_mem->operands[G_orpn->l1_npos].data.float64 /= G_orpn->nums[G_orpn->num_pos].data.float64;
                        }
                        else
                        {
                            // if the dominant op is the same, the calculation must be reversed
                            G_orpn->l1_mem->operands[G_orpn->l1_npos].data.float64 *= G_orpn->nums[G_orpn->num_pos].data.float64;
                        }
                        break;
                }                
            }
            else if (G_orpn->l1_npos < 0 || vu8_get_last(G_orpn->operators) != G_orpn->dominant_op)
            {
                // based on IEEE 754 standard it is save to compare with 0.0
                if (vu8_get_last(G_orpn->operators) == DIV && 
                 ((G_orpn->nums[G_orpn->num_pos].type == T_INT && G_orpn->nums[G_orpn->num_pos].data.integer == 0)
                 || (G_orpn->nums[G_orpn->num_pos].type == T_FLOAT && G_orpn->nums[G_orpn->num_pos].data.float64 == 0.0)))
                {
                    exit(SEMANTIC_ZERO_DIV_ERR);
                }

                // condition for partial reduction were not met, therfore the constant must be added to the L1 memory
                PUSH_L1
                G_orpn->l1_mem->operands[G_orpn->l1_mem->pos] = G_orpn->nums[G_orpn->num_pos];

                G_orpn->l1_npos = G_orpn->l1_mem->pos;
                G_orpn->vnum_first = false;

                PUSH_L1
                // setting the new dominant OP
                G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].data.op = G_orpn->dominant_op = vu8_pop(G_orpn->operators);
                G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].type = T_OP;

            }
            else
            {
                // worng types
                exit(SEMANTIC_EXPR_ERR);
            }

            G_orpn->num_pos--;
        }
        else // last added operand was an identifier
        {
            PUSH_L1
            G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].data.op = G_orpn->dominant_op = vu8_pop(G_orpn->operators);
            G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].type = T_OP;
        }
    }
    else // expression can be fully calculated
    {
        if (G_orpn->nums[G_orpn->num_pos - 1].type == T_INT && G_orpn->nums[G_orpn->num_pos].type == T_INT)
        {
            switch (vu8_pop(G_orpn->operators))
            {
                case MUL:
                    G_orpn->nums[G_orpn->num_pos - 1].data.integer *= G_orpn->nums[G_orpn->num_pos].data.integer;
                    break;

                case DIV:
                    if (G_orpn->nums[G_orpn->num_pos].data.integer == 0)
                    {
                        exit(SEMANTIC_ZERO_DIV_ERR);
                    }
                    G_orpn->nums[G_orpn->num_pos - 1].data.integer /= G_orpn->nums[G_orpn->num_pos].data.integer;
                    break;
            }
        }
        else if (G_orpn->nums[G_orpn->num_pos - 1].type == T_FLOAT && G_orpn->nums[G_orpn->num_pos].type == T_FLOAT)
        {
            switch (vu8_pop(G_orpn->operators))
            {
                case MUL:
                    G_orpn->nums[G_orpn->num_pos - 1].data.float64 *= G_orpn->nums[G_orpn->num_pos].data.float64;
                    break;

                case DIV:
                    if (G_orpn->nums[G_orpn->num_pos].data.float64 == 0.0) // based on IEEE 754 standard it is save to compare with 0.0
                    {
                        exit(SEMANTIC_ZERO_DIV_ERR);
                    }
                    G_orpn->nums[G_orpn->num_pos - 1].data.float64 /= G_orpn->nums[G_orpn->num_pos].data.float64;
                    break;
            }
        }
        else
        {
            exit(SEMANTIC_EXPR_ERR);
        }
        G_orpn->num_pos--;
    }
}

void l3_op() // evaluation of L3 operator
{
    if (G_orpn->num_pos < 0 || G_orpn->l1_vexpr)
    {
        // last added operand is variable or function
        PUSH_L1
        G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].data.op = vu8_pop(G_orpn->operators);
        G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].type = T_OP;
    }
    else // expression can be calculated
    {
        // calculation of first constant, the L1 acumulator is empty and the result is stored there
        if (G_orpn->num_pos == 1)
        {
            if (G_orpn->nums[0].type == T_INT && G_orpn->nums[1].type == T_INT)
            {
                switch (vu8_pop(G_orpn->operators))
                {
                    case ADD:
                        G_orpn->l1_mem->operands[0].data.integer = G_orpn->nums[0].data.integer + G_orpn->nums[1].data.integer;
                        break;

                    case SUB:
                        G_orpn->l1_mem->operands[0].data.integer = G_orpn->nums[0].data.integer - G_orpn->nums[1].data.integer;
                        break;
                }
                G_orpn->l1_mem->operands[0].type = T_INT;
            }
            else if (G_orpn->nums[0].type == T_FLOAT && G_orpn->nums[1].type == T_FLOAT)
            {
                switch (vu8_pop(G_orpn->operators))
                {
                    case ADD:
                        G_orpn->l1_mem->operands[0].data.float64 = G_orpn->nums[0].data.float64 + G_orpn->nums[1].data.float64;
                        break;

                    case SUB:
                        G_orpn->l1_mem->operands[0].data.float64 = G_orpn->nums[0].data.float64 - G_orpn->nums[1].data.float64;
                        break;
                }
                G_orpn->l1_mem->operands[0].type = T_FLOAT;
            }
            else if (G_orpn->nums[0].type == T_STRING && G_orpn->nums[1].type == T_STRING && vu8_pop(G_orpn->operators) == ADD)
            {
                if (G_orpn->l1_mem->pos == 0) //string concatination
                {
                    G_orpn->l1_mem->operands[0].data.string = 
                    malloc(strlen(G_orpn->nums[0].data.string) + strlen(G_orpn->nums[1].data.string) + 1);
                    if (G_orpn->l1_mem->operands[0].data.string == NULL)
                    {
                        exit(MEM_ERR);
                    }
                    strcpy(G_orpn->l1_mem->operands[0].data.string, G_orpn->nums[0].data.string);
                    strcat(G_orpn->l1_mem->operands[0].data.string, G_orpn->nums[1].data.string);
                    free(G_orpn->nums[0].data.string);
                    free(G_orpn->nums[1].data.string);
                    G_orpn->l1_mem->operands[0].type = T_STRING;
                }
                else // strings must stay in the correct order, known string literal must be added to the L1 memory
                {
                    G_orpn->l1_mem->operands[0] = G_orpn->nums[0];

                    PUSH_L1
                    G_orpn->l1_mem->operands[G_orpn->l1_mem->pos] = G_orpn->nums[1];

                    PUSH_L1
                    G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].data.op = ADD;
                    G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].type = T_OP;

                    G_orpn->num_pos = -1;
                }
            }
            else
            {
                exit(SEMANTIC_EXPR_ERR);
            }

            G_orpn->num_pos = -1;
            G_orpn->l1_frst_num = true;
        }
        // calculating the result with the already calculated result in accumulator,
        // or adding to/subtracting from the L1 accumulator with 0 value
        else 
        {
            if ((G_orpn->l1_mem->operands[0].type == T_INT && G_orpn->nums[0].type == T_INT) || 
                (G_orpn->l1_mem->operands[0].type == NUM && G_orpn->nums[0].type == T_INT))
            {
                switch (vu8_pop(G_orpn->operators))
                {
                    case ADD:
                        G_orpn->l1_mem->operands[0].data.integer += G_orpn->nums[0].data.integer;
                        break;
                    
                    case SUB:
                        G_orpn->l1_mem->operands[0].data.integer -= G_orpn->nums[0].data.integer;
                        break;
                }
            }
            else if ((G_orpn->l1_mem->operands[0].type == T_FLOAT && G_orpn->nums[0].type == T_FLOAT) || 
                     (G_orpn->l1_mem->operands[0].type == NUM && G_orpn->nums[0].type == T_FLOAT))
            {
                switch (vu8_pop(G_orpn->operators))
                {
                    case ADD:
                        G_orpn->l1_mem->operands[0].data.float64 += G_orpn->nums[0].data.float64;
                        break;
                    
                    case SUB:
                        G_orpn->l1_mem->operands[0].data.float64 -= G_orpn->nums[0].data.float64;
                        break;
                }
            }
            else if ((G_orpn->l1_mem->operands[0].type == T_STRING && G_orpn->nums[0].type == T_STRING) || 
                     (G_orpn->l1_mem->operands[0].type == NUM && G_orpn->nums[0].type == T_STRING &&
                     vu8_get_last(G_orpn->operators) == ADD))
            {
                vu8_pop(G_orpn->operators);
                if (G_orpn->l1_vin && G_orpn->l1_mem->operands[0].type == NUM && G_orpn->last_addition == VAR)
                {
                    G_orpn->l1_mem->operands[0] = G_orpn->nums[0];
                    G_orpn->num_pos--;
                    return;
                }
                else if (G_orpn->l1_vin && G_orpn->l1_mem->operands[G_orpn->l1_mem->pos - 1].type == T_STRING && 
                G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].type == T_OP)
                {
                    // string concatination
                    G_orpn->l1_mem->pos--;
                    char *tmp = realloc(G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].data.string,
                                        strlen(G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].data.string) + 
                                        strlen(G_orpn->nums[0].data.string) + 1);
                    if (tmp == NULL)
                    {
                        exit(MEM_ERR);
                    }
                    G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].data.string = tmp;
                    strcat(G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].data.string, G_orpn->nums[0].data.string);
                    G_orpn->num_pos--;
                    G_orpn->l1_mem->pos++;
                    free(G_orpn->nums[0].data.string);
                    return;
                }
                else if (G_orpn->l1_vin)
                {
                    PUSH_L1
                    G_orpn->l1_mem->operands[G_orpn->l1_mem->pos] = G_orpn->nums[0];
                    G_orpn->num_pos--;

                    PUSH_L1
                    G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].data.op = ADD;
                    G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].type = T_OP;
                    return;
                }
                else
                {
                    // more string concatination
                    char *tmp = realloc(G_orpn->l1_mem->operands[0].data.string, 
                                        strlen(G_orpn->l1_mem->operands[0].data.string) + 
                                        strlen(G_orpn->nums[0].data.string) + 1);
                    if (tmp == NULL)
                    {
                        exit(MEM_ERR);
                    }
                    G_orpn->l1_mem->operands[0].data.string = tmp;
                    strcat(G_orpn->l1_mem->operands[0].data.string, G_orpn->nums[0].data.string);
                    free(G_orpn->nums[0].data.string);
                }
            }
            else
            {
                exit(SEMANTIC_EXPR_ERR); // missmatch in data types
            }

            // set the tipe of the L1 accumulator
            G_orpn->l1_mem->operands[0].type = G_orpn->nums[0].type;
            G_orpn->num_pos--;
        }
    }
}

void l4_op() // evaluation of L4 operator
{
    if (G_orpn->relation)
    {
        exit(SYNTAX_ERR); // multiple relational operators in one subexpression
    }
    G_orpn->relation = true;

    if (G_orpn->l1_vin || G_orpn->l2_vin)
    {
        // move L2 memory to L3 memory
        for (int64_t i = 0; i <= G_orpn->l2_mem->pos; i++)
        {
            PUSH_L3
            G_orpn->l3_mem->operands[G_orpn->l3_mem->pos] = G_orpn->l2_mem->operands[i];
        }
        G_orpn->l2_mem->pos = -1; // set L2 memory empty

        // move constanst to L1 memory
        if (G_orpn->num_pos == 0)
        {
            PUSH_L3
            G_orpn->l3_mem->operands[G_orpn->l3_mem->pos] = G_orpn->nums[0];
            G_orpn->num_pos--;
        }
        
        // move L1 memory to L3 memory
        for (int64_t i = G_orpn->l1_mem->operands[0].type == NUM; i <= G_orpn->l1_mem->pos; i++)
        {
            PUSH_L3
            G_orpn->l3_mem->operands[G_orpn->l3_mem->pos] = G_orpn->l1_mem->operands[i];
        }

        if (!G_orpn->l1_frst_num && G_orpn->l1_mem->operands[0].type != NUM)
        {
            // add + if a constant was calculated
            PUSH_L3
            G_orpn->l3_mem->operands[G_orpn->l3_mem->pos].type = T_OP;
            G_orpn->l3_mem->operands[G_orpn->l3_mem->pos].data.op = ADD;
        }

        // add the correct L4 operator
        PUSH_L3
        G_orpn->l3_mem->operands[G_orpn->l3_mem->pos].type = T_OP;
        G_orpn->l3_mem->operands[G_orpn->l3_mem->pos].data.op = vu8_pop(G_orpn->operators);

        G_orpn->l3_vexpr = G_orpn->l3_vin = true; // set flags, that unknown values are in L3 memory
    }
    else // constatns that can be calculated
    {
        if (G_orpn->num_pos == 0)
        {
            G_orpn->l1_mem->operands[0] = G_orpn->nums[0];
            G_orpn->num_pos = -1;
        }

        PUSH_L3 //store the result to L3 memory

        if (G_orpn->l2_mem->operands[0].type == T_INT && G_orpn->l1_mem->operands[0].type == T_INT)
        {
            switch (vu8_pop(G_orpn->operators))
            {
                case LT:
                    G_orpn->l3_mem->operands[G_orpn->l3_mem->pos].data.boolean = 
                    G_orpn->l2_mem->operands[0].data.integer < G_orpn->l1_mem->operands[0].data.integer;
                    break;

                case LTE:
                    G_orpn->l3_mem->operands[G_orpn->l3_mem->pos].data.boolean = 
                    G_orpn->l2_mem->operands[0].data.integer <= G_orpn->l1_mem->operands[0].data.integer;
                    break;

                case GT:
                    G_orpn->l3_mem->operands[G_orpn->l3_mem->pos].data.boolean = 
                    G_orpn->l2_mem->operands[0].data.integer > G_orpn->l1_mem->operands[0].data.integer;
                    break;

                case GTE:
                    G_orpn->l3_mem->operands[G_orpn->l3_mem->pos].data.boolean = 
                    G_orpn->l2_mem->operands[0].data.integer >= G_orpn->l1_mem->operands[0].data.integer;
                    break;

                case EQ:
                    G_orpn->l3_mem->operands[G_orpn->l3_mem->pos].data.boolean = 
                    G_orpn->l2_mem->operands[0].data.integer == G_orpn->l1_mem->operands[0].data.integer;
                    break;

                case NEQ:
                    G_orpn->l3_mem->operands[G_orpn->l3_mem->pos].data.boolean = 
                    G_orpn->l2_mem->operands[0].data.integer != G_orpn->l1_mem->operands[0].data.integer;
                    break;
            }
        }
        else if (G_orpn->l2_mem->operands[0].type == T_FLOAT && G_orpn->l1_mem->operands[0].type == T_FLOAT)
        {
            switch (vu8_pop(G_orpn->operators))
            {
                case LT:
                    G_orpn->l3_mem->operands[G_orpn->l3_mem->pos].data.boolean = 
                    G_orpn->l2_mem->operands[0].data.float64 < G_orpn->l1_mem->operands[0].data.float64;
                    break;

                case LTE:
                    G_orpn->l3_mem->operands[G_orpn->l3_mem->pos].data.boolean = 
                    G_orpn->l2_mem->operands[0].data.float64 <= G_orpn->l1_mem->operands[0].data.float64;
                    break;

                case GT:
                    G_orpn->l3_mem->operands[G_orpn->l3_mem->pos].data.boolean = 
                    G_orpn->l2_mem->operands[0].data.float64 > G_orpn->l1_mem->operands[0].data.float64;
                    break;

                case GTE:
                    G_orpn->l3_mem->operands[G_orpn->l3_mem->pos].data.boolean = 
                    G_orpn->l2_mem->operands[0].data.float64 >= G_orpn->l1_mem->operands[0].data.float64;
                    break;

                case EQ:
                    G_orpn->l3_mem->operands[G_orpn->l3_mem->pos].data.boolean = 
                    G_orpn->l2_mem->operands[0].data.float64 == G_orpn->l1_mem->operands[0].data.float64;
                    break;

                case NEQ:
                    G_orpn->l3_mem->operands[G_orpn->l3_mem->pos].data.boolean = 
                    G_orpn->l2_mem->operands[0].data.float64 != G_orpn->l1_mem->operands[0].data.float64;
                    break;
            }
        }
        else if (G_orpn->l2_mem->operands[0].type == T_STRING && G_orpn->l1_mem->operands[0].type == T_STRING)
        {
            switch (vu8_pop(G_orpn->operators))
            {
                case LT:
                    G_orpn->l3_mem->operands[G_orpn->l3_mem->pos].data.boolean = 
                    strcmp(G_orpn->l2_mem->operands[0].data.string, G_orpn->l1_mem->operands[0].data.string) < 0;
                    break;

                case LTE:
                    G_orpn->l3_mem->operands[G_orpn->l3_mem->pos].data.boolean = 
                    strcmp(G_orpn->l2_mem->operands[0].data.string, G_orpn->l1_mem->operands[0].data.string) <= 0;
                    break;

                case GT:
                    G_orpn->l3_mem->operands[G_orpn->l3_mem->pos].data.boolean = 
                    strcmp(G_orpn->l2_mem->operands[0].data.string, G_orpn->l1_mem->operands[0].data.string) > 0;
                    break;

                case GTE:
                    G_orpn->l3_mem->operands[G_orpn->l3_mem->pos].data.boolean = 
                    strcmp(G_orpn->l2_mem->operands[0].data.string, G_orpn->l1_mem->operands[0].data.string) >= 0;
                    break;

                case EQ:
                    G_orpn->l3_mem->operands[G_orpn->l3_mem->pos].data.boolean = 
                    strcmp(G_orpn->l2_mem->operands[0].data.string, G_orpn->l1_mem->operands[0].data.string) == 0;
                    break;

                case NEQ:
                    G_orpn->l3_mem->operands[G_orpn->l3_mem->pos].data.boolean = 
                    strcmp(G_orpn->l2_mem->operands[0].data.string, G_orpn->l1_mem->operands[0].data.string) != 0;
                    break;
            }
            // strings memory deallocation as comaparing creates boolean value and string literals are not needed anymore
            free(G_orpn->l2_mem->operands[0].data.string);
            free(G_orpn->l1_mem->operands[0].data.string);
        }
        else if (G_orpn->l2_mem->operands[0].type == T_BOOL && G_orpn->l1_mem->operands[0].type == T_BOOL)
        {
            switch (vu8_pop(G_orpn->operators))
            {
                case EQ:
                    G_orpn->l3_mem->operands[G_orpn->l3_mem->pos].data.boolean = 
                    G_orpn->l2_mem->operands[0].data.float64 == G_orpn->l1_mem->operands[0].data.float64;
                    break;

                case NEQ:
                    G_orpn->l3_mem->operands[G_orpn->l3_mem->pos].data.boolean = 
                    G_orpn->l2_mem->operands[0].data.float64 != G_orpn->l1_mem->operands[0].data.float64;
                    break;

                default:
                    exit(SEMANTIC_EXPR_ERR); // boolan can be only compared by == and !=
            }
        }
        else
        {
            exit(SEMANTIC_EXPR_ERR);
        }

        G_orpn->l3_mem->operands[G_orpn->l3_mem->pos].type = T_BOOL; // set the resulting type in L3 memory
    }

    // reset lower memories
    G_orpn->num_pos = -1;
    G_orpn->l1_mem->pos = 0;
    G_orpn->l1_mem->operands[0].data = (token_data_t){0,};
    G_orpn->l1_mem->operands[0].type = NUM;
    G_orpn->l2_mem->pos = -1;
    G_orpn->l1_vexpr = false;
    G_orpn->l2_vin = false;
    G_orpn->l1_vin = false;
    G_orpn->l1_frst_num = false;
}

void l5_op() // evaluation of L5 operator
{
    move_data(AND); //necessery if there was single boolean value

    if (G_orpn->l3_vexpr) // unknown value is beeing calculated
    {
        if (G_orpn->l3_mem->operands[G_orpn->l3_mem->pos].type == T_BOOL && 
            G_orpn->l3_mem->operands[G_orpn->l3_mem->pos].data.boolean == true)
        {
            // && true -> can be omitted
            G_orpn->l3_mem->pos--;
            vu8_pop(G_orpn->operators);
            G_orpn->l3_mem->operands[0].type = T_BOOL;
        }
        else
        {
            PUSH_L3
            G_orpn->l3_mem->operands[G_orpn->l3_mem->pos].data.op = vu8_pop(G_orpn->operators);
            G_orpn->l3_mem->operands[G_orpn->l3_mem->pos].type = T_OP;
        }
    }
    else
    {
        vu8_pop(G_orpn->operators);
        
        if (G_orpn->l3_mem->operands[G_orpn->l3_mem->pos].type == T_BOOL)
        {
            // && with previously stored
            G_orpn->l3_mem->operands[G_orpn->l3_mem->pos - 1].data.boolean = 
            G_orpn->l3_mem->operands[G_orpn->l3_mem->pos - 1].data.boolean && 
            G_orpn->l3_mem->operands[G_orpn->l3_mem->pos].data.boolean;
            G_orpn->l3_mem->pos--;
        }
        else
        {
            exit(SEMANTIC_EXPR_ERR); //type missmatch
        }
    }
}

void l6_op() // evaluation of L6 operator
{
    move_data(SPECIFIC_OR); //necessery if there was single boolean value

    if (G_orpn->l3_vexpr) // unknown value
    {
        PUSH_L3
        G_orpn->l3_mem->operands[G_orpn->l3_mem->pos].data.op = vu8_pop(G_orpn->operators);
        G_orpn->l3_mem->operands[G_orpn->l3_mem->pos].type = T_OP;
    }
    else
    {
        vu8_pop(G_orpn->operators);
        // L3 accumulator is empty
        if (G_orpn->l3_mem->operands[0].type == NUM && G_orpn->l3_mem->pos == 2 && 
            G_orpn->l3_mem->operands[G_orpn->l3_mem->pos - 1].type == T_BOOL &&
            G_orpn->l3_mem->operands[G_orpn->l3_mem->pos].type == T_BOOL)
        {
            // set the L3 accumulator
            G_orpn->l3_mem->operands[0].data.boolean =
            G_orpn->l3_mem->operands[G_orpn->l3_mem->pos - 1].data.boolean || 
            G_orpn->l3_mem->operands[G_orpn->l3_mem->pos].data.boolean;
            G_orpn->l3_mem->pos -= 2;
            G_orpn->l3_mem->operands[0].type = T_BOOL;
        }
        else if (G_orpn->l3_mem->operands[G_orpn->l3_mem->pos].type == T_BOOL)
        {
            // OR with the L3 accumulator
            G_orpn->l3_mem->operands[0].data.boolean =
            G_orpn->l3_mem->operands[0].data.boolean || G_orpn->l3_mem->operands[G_orpn->l3_mem->pos].data.boolean;
            G_orpn->l3_mem->pos--;
            G_orpn->l3_mem->operands[0].type = T_BOOL;
        }
        else
        {
            exit(SEMANTIC_EXPR_ERR); //type missmatch
        }
    }
    G_orpn->l3_vexpr = false;
}

void l7_op() // evaluation of L7 operator
{
    if (G_data_orpn->pos > 0)
    {
        exit(SYNTAX_ERR); //wrong count of brackets
    }

    switch(vu8_pop(G_orpn->operators))
    {
        case ADD_EQ:
            //constant value calculated and variable also has constant value
            if (is_var_assignable() && has_var_value(G_orpn->l1_mem->operands[1].data.var) && G_orpn->l3_mem->pos == 1)
            {
                // assigned variable is always stored in G_orpn->l3_mem->operands[1]
                if (G_orpn->l3_mem->operands[1].type == T_INT)
                {
                    set_var_value(G_orpn->l1_mem->operands[1].data.var, 
                    (var_values_t){.integer = get_var_value(G_orpn->l1_mem->operands[1].data.var).integer + G_orpn->l3_mem->operands[1].data.integer}, T_INT);
                    return;
                }
                else if (G_orpn->l3_mem->operands[1].type == T_FLOAT)
                {
                    set_var_value(G_orpn->l1_mem->operands[1].data.var, 
                    (var_values_t){.float64 = get_var_value(G_orpn->l1_mem->operands[1].data.var).float64 + G_orpn->l3_mem->operands[1].data.float64}, T_FLOAT);
                    return;
                }
                else if (G_orpn->l3_mem->operands[1].type == T_STRING)
                {
                    // string concatination
                    char *hold = malloc(strlen(get_var_value(G_orpn->l1_mem->operands[1].data.var).string) + strlen(G_orpn->l3_mem->operands[1].data.string) + 1);
                    if (hold == NULL)
                    {
                        exit(MEM_ERR);
                    }
                    strcpy(hold, get_var_value(G_orpn->l1_mem->operands[1].data.var).string);
                    strcat(hold, G_orpn->l3_mem->operands[1].data.string);
                    free(G_orpn->l3_mem->operands[1].data.string);
                    set_var_value(G_orpn->l1_mem->operands[1].data.var, (var_values_t){.string = hold}, T_STRING);
                    return;   
                }
            }

            // either variable or the calculation is not a constant value
            unset_var_value(G_orpn->l1_mem->operands[1].data.var);
            lvar_insert(G_orpn->l1_mem->operands[1].data.var); // variable because of the += operator
            PUSH_L3
            G_orpn->l3_mem->operands[G_orpn->l3_mem->pos] = (operand_t) {T_OP, (token_data_t){.op = ADD}};
            load_to_list(); // load stored token to the token stream
            break;

        case MUL_EQ: // same as ADD_EQ without concatination
            if (is_var_assignable() && has_var_value(G_orpn->l1_mem->operands[1].data.var) && G_orpn->l3_mem->pos == 1)
            {
                if (G_orpn->l3_mem->operands[1].type == T_INT)
                {
                    set_var_value(G_orpn->l1_mem->operands[1].data.var, 
                    (var_values_t){.integer = get_var_value(G_orpn->l1_mem->operands[1].data.var).integer * G_orpn->l3_mem->operands[1].data.integer}, T_INT);
                    return;
                }
                else if (G_orpn->l3_mem->operands[1].type == T_FLOAT)
                {
                    set_var_value(G_orpn->l1_mem->operands[1].data.var, 
                    (var_values_t){.float64 = get_var_value(G_orpn->l1_mem->operands[1].data.var).float64 * G_orpn->l3_mem->operands[1].data.float64}, T_FLOAT);
                    return;
                }
            }

            unset_var_value(G_orpn->l1_mem->operands[1].data.var);
            lvar_insert(G_orpn->l1_mem->operands[1].data.var);
            PUSH_L3
            G_orpn->l3_mem->operands[G_orpn->l3_mem->pos] = (operand_t) {T_OP, (token_data_t){.op = MUL}};
            load_to_list();
            break;

        case SUB_EQ: // same as MUL_EQ
            if (is_var_assignable() && has_var_value(G_orpn->l1_mem->operands[1].data.var) && G_orpn->l3_mem->pos == 1)
            {
                if (G_orpn->l3_mem->operands[1].type == T_INT)
                {
                    set_var_value(G_orpn->l1_mem->operands[1].data.var, 
                    (var_values_t){.integer = get_var_value(G_orpn->l1_mem->operands[1].data.var).integer - G_orpn->l3_mem->operands[1].data.integer}, T_INT);
                    return;
                }
                else if (G_orpn->l3_mem->operands[1].type == T_FLOAT)
                {
                    set_var_value(G_orpn->l1_mem->operands[1].data.var, 
                    (var_values_t){.float64 = get_var_value(G_orpn->l1_mem->operands[1].data.var).float64 - G_orpn->l3_mem->operands[1].data.float64}, T_FLOAT);
                    return;
                }
            }
            
            unset_var_value(G_orpn->l1_mem->operands[1].data.var);
            lvar_insert(G_orpn->l1_mem->operands[1].data.var);
            PUSH_L3
            G_orpn->l3_mem->operands[G_orpn->l3_mem->pos] = (operand_t) {T_OP, (token_data_t){.op = SUB}};
            load_to_list();
            break;

        case DIV_EQ: // same as MUL_EQ
            if (is_var_assignable() && has_var_value(G_orpn->l1_mem->operands[1].data.var) && G_orpn->l3_mem->pos == 1)
            {
                if (G_orpn->l3_mem->operands[1].type == T_INT)
                {
                    if (G_orpn->l3_mem->operands[1].data.float64 == 0.0)
                    {
                        exit(SEMANTIC_ZERO_DIV_ERR);
                    }
                    set_var_value(G_orpn->l1_mem->operands[1].data.var, 
                    (var_values_t){.integer = get_var_value(G_orpn->l1_mem->operands[1].data.var).integer / G_orpn->l3_mem->operands[1].data.integer}, T_INT);
                    return;
                }
                else if (G_orpn->l3_mem->operands[1].type == T_FLOAT)
                {
                    if (G_orpn->l3_mem->operands[1].data.float64 == 0.0)
                    {
                        exit(SEMANTIC_ZERO_DIV_ERR);
                    }
                    set_var_value(G_orpn->l1_mem->operands[1].data.var, 
                    (var_values_t){.float64 = get_var_value(G_orpn->l1_mem->operands[1].data.var).float64 / G_orpn->l3_mem->operands[1].data.float64}, T_FLOAT);
                    return;
                }
            }
            
            unset_var_value(G_orpn->l1_mem->operands[1].data.var);
            lvar_insert(G_orpn->l1_mem->operands[1].data.var);
            PUSH_L3
            G_orpn->l3_mem->operands[G_orpn->l3_mem->pos] = (operand_t) {T_OP, (token_data_t){.op = DIV}};
            load_to_list();
            break;

        case RETURN_END:
        case COMMA:
        case COND_EXPR_END:
                // load the expression to the token stream
                load_to_list();
            break;
        
        case EQ_DEF:
        case EQ_ASSGN:
            if (is_var_assignable()) // it is posibble to assign to the variable
            {
                // the value is known if only the L3 memory accumulator is used, then assign the correct type
                if (G_orpn->l3_mem->operands[0].type == T_BOOL && G_orpn->l3_mem->pos == 0)
                {
                    set_var_value(G_orpn->l1_mem->operands[1].data.var, 
                        (var_values_t){.boolean = G_orpn->l3_mem->operands[0].data.boolean}, T_BOOL);
                    return;
                }
                else if (G_orpn->l3_mem->pos == 1 && G_orpn->l3_mem->operands[0].type == NUM && G_orpn->l3_mem->operands[1].type == T_BOOL)
                {
                    set_var_value(G_orpn->l1_mem->operands[1].data.var, 
                        (var_values_t){.boolean = G_orpn->l3_mem->operands[1].data.boolean}, T_BOOL);
                    return;
                }
                else if (G_orpn->l3_mem->pos == 1 && G_orpn->l3_mem->operands[1].type == T_INT)
                {
                    set_var_value(G_orpn->l1_mem->operands[1].data.var, 
                        (var_values_t){.integer = G_orpn->l3_mem->operands[1].data.integer}, T_INT);
                    return;
                }
                else if (G_orpn->l3_mem->pos == 1 && G_orpn->l3_mem->operands[1].type == T_FLOAT)
                {
                    set_var_value(G_orpn->l1_mem->operands[1].data.var, 
                        (var_values_t){.float64 = G_orpn->l3_mem->operands[1].data.float64}, T_FLOAT);
                    return;
                }
                else if (G_orpn->l3_mem->pos == 1 && G_orpn->l3_mem->operands[1].type == T_STRING)
                {
                    set_var_value(G_orpn->l1_mem->operands[1].data.var, 
                        (var_values_t){.string = G_orpn->l3_mem->operands[1].data.string}, T_STRING);
                    return;
                }
            }
            
            // preemtively unset the variable value
            unset_var_value(G_orpn->l1_mem->operands[1].data.var);
            // load the tokens to the token stream
            load_to_list();
            break;
        
        case PARAM_COMMA:
            // wait for another parameters
            break;

        default:
            break;
    }
}

void move_data(uint8_t op)
{
    if (op >> OP_CODE_SHIFT == L3_OP)
    {
        // reset the flasg
        G_orpn->l1_vexpr = false; // unknown expression end
        G_orpn->vnum_first = false; // not first costant anymore
        G_orpn->l1_npos = -1; // reset the position of a constant
    } 
    else if (op >> OP_CODE_SHIFT == L4_OP)
    {
        // move constant to L1
        if (G_orpn->num_pos >= 0)
        {
            G_orpn->l1_mem->operands[0] = G_orpn->nums[0];
            G_orpn->l1_frst_num = true;
        }

        // move L1 memory to L2 memory
        for (int64_t i = G_orpn->l1_mem->operands[0].type == NUM; i <= G_orpn->l1_mem->pos; i++)
        {
            PUSH_L2
            G_orpn->l2_mem->operands[G_orpn->l2_mem->pos] = G_orpn->l1_mem->operands[i];
        }

        // add + if fisrt constant is present
        if (!G_orpn->l1_frst_num && G_orpn->l1_mem->operands[0].type != NUM)
        {
            PUSH_L2
            G_orpn->l2_mem->operands[G_orpn->l2_mem->pos].type = T_OP;
            G_orpn->l2_mem->operands[G_orpn->l2_mem->pos].data = (token_data_t){.op = ADD};
        }
        
        // set/reset flags
        G_orpn->l2_vin = G_orpn->l1_vin;
        G_orpn->l1_mem->operands[0].type = NUM;
        G_orpn->l1_mem->operands[0].data = (token_data_t){0, };
        G_orpn->l1_mem->pos = 0;
        G_orpn->num_pos = -1;
        G_orpn->l1_vin = false;
        G_orpn->l1_vexpr = false;
        G_orpn->l1_frst_num = false;
        G_orpn->vnum_first = false;
        G_orpn->l1_npos = -1;
    }
    else if (op >> OP_CODE_SHIFT >= L5_OP)
    {
        // move constant to L1
        if (G_orpn->num_pos >= 0)
        {
            G_orpn->l1_mem->operands[0] = G_orpn->nums[0];
            G_orpn->l1_frst_num = true;
        }

        // unknown value in L1 memory
        if (G_orpn->l1_mem->pos > 0)
        {
            G_orpn->l3_vexpr = true;
        }

        // move from L1 to L3 directly
        for (int64_t i = G_orpn->l1_mem->operands[0].type == NUM; i <= G_orpn->l1_mem->pos; i++)
        {
            PUSH_L3
            G_orpn->l3_mem->operands[G_orpn->l3_mem->pos] = G_orpn->l1_mem->operands[i];
        }

        if (!G_orpn->l1_frst_num && G_orpn->l1_mem->operands[0].type != NUM)
        {
            PUSH_L3
            G_orpn->l3_mem->operands[G_orpn->l3_mem->pos].type = T_OP;
            G_orpn->l3_mem->operands[G_orpn->l3_mem->pos].data = (token_data_t){.op = ADD};
        }

        // set/reset flags
        G_orpn->l3_vin = G_orpn->l3_vin || G_orpn->l1_vin;
        G_orpn->l1_mem->operands[0].type = NUM;
        G_orpn->l1_mem->operands[0].data = (token_data_t){0, };
        G_orpn->l1_mem->pos = 0;
        G_orpn->num_pos = -1;
        G_orpn->l1_vin = false;
        G_orpn->l1_vexpr = false;
        G_orpn->l1_frst_num = false;
        G_orpn->relation = false;
        G_orpn->vnum_first = false;
        G_orpn->l1_npos = -1;

        if (op == OR)
        {
            G_orpn->l3_vexpr = false;
        }
        else if (op == PARAM_COMMA) // add tokens representing function call
        {
            PUSH_L3
            G_orpn->l3_mem->operands[G_orpn->l3_mem->pos].type = T_PARAM_END;
            PUSH_L3
            G_orpn->l3_mem->operands[G_orpn->l3_mem->pos].type = T_PARAM;
            vu8_pop(G_orpn->operators);
        }
    }
}

void load_to_list()
{
    // skip the L3 accumulator if it is empty or false value
    for (int64_t i = G_orpn->l3_mem->operands[0].type != T_BOOL || G_orpn->l3_mem->operands[0].data.boolean == false; 
        i <= G_orpn->l3_mem->pos; i++)
    {
        // load appropriately
        switch(G_orpn->l3_mem->operands[i].type)
        {
            case T_INT:
                lint_insert(G_orpn->l3_mem->operands[i].data.integer);
                break;

            case VAR:
                lvar_insert(G_orpn->l3_mem->operands[i].data.var);
                break;

            case T_OP:
                lop_insert(G_orpn->l3_mem->operands[i].data.op);
                break;

            case T_BOOL:
                lbool_insert(G_orpn->l3_mem->operands[i].data.boolean);
                break;

            case T_STRING:
                lstring_insert(G_orpn->l3_mem->operands[i].data.string);
                break;

            case T_FLOAT:
                lfloat_insert(G_orpn->l3_mem->operands[i].data.float64);
                break;

            case FUNC:
                lfunc_insert(G_orpn->l3_mem->operands[i].data.func);
                break;
            
            case T_PARAM:
                if (G_orpn->l3_mem->operands[i + 1].type != T_PARAM_END)
                {
                    ltype_insert(G_orpn->l3_mem->operands[i].type);
                }
                else
                {
                    i++;
                }
                break;

            default:
                ltype_insert(G_orpn->l3_mem->operands[i].type);
                break;
        }
    }

    if (G_orpn->l3_mem->operands[0].type == T_BOOL && G_orpn->l3_mem->pos > 0)
    {
        if (G_orpn->l3_mem->operands[0].data.boolean)
        {
            lop_insert(OR); // insert OR operator in case of L3 accumulator is true
        }
        else
        {
            lop_insert(POS_NOT); // ensure boolean value during semantic analysis, POS_NOT has no other effect
        }
    }
    else if (G_orpn->l3_mem->operands[0].type == T_BOOL)
    {
        if (G_orpn->l3_mem->operands[0].data.boolean == false && G_orpn->l3_mem->pos == 0)
        {
            lbool_insert(false); // the expression result is just false
        }
        lop_insert(POS_NOT); // ensure correct semantic analysis
    }
}

void orpn_expr_end()
{
    // no more operators will be added and the L7 must be the only operator on the operator stack,
    // becuase it must have been added before and it has the lowest precedence, therofore all other
    // operators were evaluated
    l7_op();
    reset_calculation();
}

void reset_calculation()
{
    G_orpn->l1_npos = STACK_START_POS;
    G_orpn->l3_npos = STACK_START_POS;
    G_orpn->l1_mem->operands[0].type = NUM;
    G_orpn->l1_mem->operands[0].data = (token_data_t){0, };
    G_orpn->l3_mem->operands[0].type = NUM;
    G_orpn->l3_mem->operands[0].data = (token_data_t){0, };
    G_orpn->num_pos = STACK_START_POS;
    G_orpn->dominant_op = EMPTY;
    vu8_reset(G_orpn->operators);
    vu8_push(&G_orpn->operators, EMPTY);
    G_orpn->l1_mem->pos = STACK_START_POS + 1;
    G_orpn->l2_mem->pos = STACK_START_POS;
    G_orpn->l3_mem->pos = STACK_START_POS + 1;
    G_orpn->l1_vexpr = false;
    G_orpn->l3_vexpr = false;
    G_orpn->l1_vin = false;
    G_orpn->l2_vin = false;
    G_orpn->l3_vin = false;
    G_orpn->vnum_first = false;
    G_orpn->relation = false;
    G_orpn->l1_frst_num = false;
}

void l_err_op()
{
    exit(INTERNAL_ERR);
}

// opening bracket is not added as an operator, but separately, as it results in data structure manipulation
void orpn_add_ob()
{
    if (vu8_get_last(G_orpn->operators) == IDENTIFIER)
    {
        exit(SYNTAX_ERR); //cannot follow constant, identifier or function call
    }
    
    vu8_push(&G_orpn->operators, IDENTIFIER); // bracket expression behaves as an identifier on the lower scope
    
    void *tmp;
    // creating new scope to calculate the expression inside the bracket
    if (++G_data_orpn->pos == G_data_orpn->size)
    {
        G_data_orpn->size *= 2;
        tmp = realloc(G_data_orpn, sizeof(orpn_stack_t) + G_data_orpn->size * sizeof(expr_data_t));
        if (tmp == NULL)
        {
            exit(MEM_ERR);
        }
        G_data_orpn = tmp;
    }

    // switching to the new scope
    G_orpn = &G_data_orpn->data[G_data_orpn->pos];
    if (G_data_orpn->pos > G_data_orpn->allocated)
    {
        // constructing the scope had it not been used previously
        data_const();
        G_data_orpn->allocated++;
    }
}

bool orpn_add_cb()
{
    if (G_data_orpn->pos == 0)
    {
        exit(SYNTAX_ERR); // too many closed brackets, this is already the lowes scope
    }
    else if (vu8_get_last(G_orpn->operators) == EMPTY)
    {
        // if it's not a function call
        if (!G_data_orpn->data[G_data_orpn->pos - 1].func_end_op)
        {
            exit(SYNTAX_ERR);  // closed bracket right after opened one 
        }
    }
    else
    {
        orpn_add_operator(CBRACKET); // adding the operator of close bracket here
        // all operators must be evaluated, as it has the lowest precedence
    }

    uint8_t neg = 0;

    if (G_orpn->l3_mem->pos == 1 && G_orpn->l3_mem->operands[0].type == NUM && !G_orpn->l3_vin && !G_data_orpn->data[G_data_orpn->pos - 1].func_end_op)
    {
        // expression inside brackets was a constant -> moving it to the lower scope
        G_data_orpn->data[G_data_orpn->pos - 1].nums[++G_data_orpn->data[G_data_orpn->pos - 1].num_pos] = 
        G_orpn->l3_mem->operands[1];
        G_data_orpn->data[G_data_orpn->pos - 1].last_addition = NUM;
    }
    else if (G_orpn->l3_mem->pos == 0 && G_orpn->l3_mem->operands[0].type == T_BOOL)
    {
        // expression inside brackets was a constant -> moving it to the lower scope
        G_data_orpn->data[G_data_orpn->pos - 1].nums[++G_data_orpn->data[G_data_orpn->pos - 1].num_pos] = 
        G_orpn->l3_mem->operands[0];
        G_data_orpn->data[G_data_orpn->pos - 1].last_addition = NUM;
    }
    else
    {
        // temporarily switching to the lower scope for easier code
        G_orpn = &G_data_orpn->data[G_data_orpn->pos - 1];
        // poping the IDF symbol from the stack of the lower scope
        vu8_pop(G_orpn->operators);

        // solving negation in front of the bracket
        if (vu8_get_last(G_orpn->operators) == NEG && G_orpn->func_end_op)
        {
            neg = 1;
            vu8_pop(G_orpn->operators);
        }
        if (vu8_get_last(G_orpn->operators) == NEG)
        {
            neg = 1;
            vu8_pop(G_orpn->operators);
            PUSH_L1
            G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].type = T_OP;
            // as the instructions set does not offer instruction NEG, correct zero constant must be inserted
            // in front of the bracket and NEG can be replaced by SUB
            G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].data.op = ZERO;
        }

        if (vu8_get_last(G_orpn->operators) >> OP_CODE_SHIFT == L2_OP && !G_orpn->l1_vexpr) // solve L2 operator
        {
            // set dominant operator of the subexpression
            G_orpn->dominant_op = vu8_get_last(G_orpn->operators);
            G_orpn->vnum_first = true;

            // move constant asociated with the bracket to the L1 memory 
            PUSH_L1
            G_orpn->l1_mem->operands[G_orpn->l1_mem->pos] = G_orpn->nums[G_orpn->num_pos];
            G_orpn->l1_npos = G_orpn->l1_mem->pos;
            G_orpn->num_pos--;

            if (G_data_orpn->data[G_data_orpn->pos].l3_mem->operands[0].type == T_BOOL)
            {
                exit(SEMANTIC_EXPR_ERR); // missmatch in types, aplying *,/ on boolean value
            }

            // load the subexpression from the higher scope L3 memory to the lower scope L1 memory
            for (int64_t i = 1; i <= G_data_orpn->data[G_data_orpn->pos].l3_mem->pos; i++)
            {
                PUSH_L1
                G_orpn->l1_mem->operands[G_orpn->l1_mem->pos] = G_data_orpn->data[G_data_orpn->pos].l3_mem->operands[i];
            }

            // add the operator directly associated with the bracket
            PUSH_L1
            G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].data.op = vu8_pop(G_orpn->operators);
            G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].type = T_OP;

            // set flags
            G_orpn->l1_vin = G_orpn->l1_vexpr = true;
            G_orpn->last_addition = VAR;
        }
        else // other operators
        {
            if (vu8_get_last(G_orpn->operators) >> OP_CODE_SHIFT <= L3_OP &&
                vu8_get_last(G_orpn->operators) != NOT &&
                G_data_orpn->data[G_data_orpn->pos].l3_mem->operands[0].type == T_BOOL)
            {
                exit(SEMANTIC_EXPR_ERR); // missmatch in types
            }

            // unset flags
            G_orpn->vnum_first = false;
            G_orpn->l1_npos = -1;

            // load from L3 memory of the higher scope to the L1 momory of the lower one
            for (int64_t i = G_data_orpn->data[G_data_orpn->pos].l3_mem->operands[0].type == NUM; 
            i <= G_data_orpn->data[G_data_orpn->pos].l3_mem->pos; i++)
            {
                PUSH_L1
                G_orpn->l1_mem->operands[G_orpn->l1_mem->pos] = G_data_orpn->data[G_data_orpn->pos].l3_mem->operands[i];
            }
            
            // set flags
            G_orpn->l1_vin = G_orpn->l1_vexpr = true;
            G_orpn->last_addition = VAR;

            if (G_data_orpn->data[G_data_orpn->pos].l3_mem->operands[0].type == T_BOOL && G_data_orpn->data[G_data_orpn->pos].l3_mem->pos > 0)
            {
                // add or in case of L3 accumulator is not empty
                PUSH_L1
                G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].type = T_OP;
                G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].data.op = OR;
            }

        }

        vu8_push(&G_orpn->operators, IDENTIFIER); // represent the bracket expression as an identfier -> operator must follow

        if (neg && !G_orpn->func_end_op) // solve the negation if it is not the end on an function call
        {
            PUSH_L1
            G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].type = T_OP;
            // replace NEG with SUB
            G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].data.op = SUB;
        }

        // switch back to the current scope
        G_orpn = &G_data_orpn->data[G_data_orpn->pos];
    }

    // reset current scope
    reset_calculation();
    G_data_orpn->pos--;

    // switch to the lower scope
    G_orpn = &G_data_orpn->data[G_data_orpn->pos];

    if (G_orpn->func_end_op) // close bracket is the end of a function call
    {
        // add special tokens for semantic analysis and generation
        PUSH_L1
        G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].type = T_PARAM_END;
        PUSH_L1
        G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].type = T_FCALL_END;

        if (neg) // adding the NEG operator represented by SUB
        {
            PUSH_L1
            G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].type = T_OP;
            G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].data.op = SUB;
        }

        if (G_orpn->func_end_op > 1)
        {
            // adding L2 operator bound to the function
            PUSH_L1
            G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].type = T_OP;
            G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].data.op = G_orpn->func_end_op;
        }
        G_orpn->func_end_op = 0;
        return true; // signilize the parser, that function call ended
    }
    return false; // ordinary bracket expression end
}

void orpn_add_assgn_var(htab_var_t var)
{
    // add the variable which is going to be assigned
    PUSH_L1
    G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].data.var = var;
    G_orpn->l1_mem->operands[G_orpn->l1_mem->pos].type = VAR;
}
