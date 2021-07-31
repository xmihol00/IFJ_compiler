
#include "semantic_private.h"

//=========================================================================================================
// File:        semantic.c
// Case:        VUT, FIT, IFJ, compiler project
// Date:        9. 12. 2020
// Author(s):   David Mihola
// Contac(s):   xmihol00@stud.fit.vutbr.cz
// Compiled:    gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)
// Description: File containing defintions of functions responsible for 
//              the semantic analysis of the source code
//==========================================================================================================

struct 
{
    bool last;                      // expression must end, when the value is true
    bool multi;                     // multi assign expression possible, when the value is true
    bool ret;                       // return expression, when the value is true
    bool func_multi;                // function with mutiple return types, when the value is true
    htab_func_t func;               // currently called function
    htab_func_t defined_func;       // currently defined function
    vect_uint8_t types;             // stack of types of canstants/variables in expression
    vect_htab_func_t functions;     // stack of called function
} G_control = {0,};


void init_semantic()
{
    G_control.types = vect_uint8_const();
    G_control.functions = vect_htab_func_const();
}

void dispose_sematic()
{
    discard_uint8_vector(&G_control.types);
    discard_htab_func_vector(&G_control.functions);
    
}

void semantic_analysis()
{
    init_semantic();

    lreader_pre_head(); //sets the reader of the token stream to correct position

    do
    {
        switch (lreader_n_type())
        {
            case T_FUNC_DEF: // start of function definition
                lreader_next();
                G_control.defined_func = lreader_n_data().func;
                break;

            case T_MULTI_ASSGN: // start of multi assignment
                G_control.last = false;
                lreader_skip_until(T_VARS_END);
                cadd_multi_assgn_start();
                break;
            
            case T_MULTI_ASSGN_END: // end of multi assignment
                cadd_multi_assgn_end();
                break;


            case T_FOR2: // start of for condition
            case T_IF:   // start of if condition
            case T_ELIF: // start of else if condition
                G_control.last = false;
                break;
            
            case T_IF_BODY: // end of if condition, start of if statement
                cadd_cnd_end();
                break;
            
            case T_ELIF_BODY: // end of else if condition, start of if statement
                cadd_cnd_end();
                break;

            case T_IF_BODY_END:  // end of if statement
            case T_ELIF_BODY_END:
            case T_ELSE_BODY_END:
                G_control.last = false;
                break;

            case T_ASSGN: // start of assignment
                lreader_next();
                G_control.last = false;
                break;

            case T_FOR1_EMPTY: // empty for 3 
                break;
            
            case T_FOR_BODY: // end of for condition, the for3 statement is moved at the end of the for cycle
                cadd_cnd_end();
                break;

            case T_FOR_BODY_END: // end of for statement
                G_control.last = false;
                break;

            case T_RETURN: // start of return statement
                G_control.last = false;
                G_control.ret = true;
                G_control.multi = true;
                break;
            
            case T_RETURN_END: // end of return statement
                cadd_return_end();
                break;
            
            case T_FUNC_BODY_END: // end of currently defined function
                G_control.last = false;
                break;
            
            case T_VAR_DEF: // variable definition, semantic control is skiped
                lreader_next();
                G_control.last = false;
                break;

            case T_INT: // integer value
                cadd_type(T_INT);
                break;

            case T_FLOAT: // floating point value
                cadd_type(T_FLOAT);
                break;

            case T_BOOL: // boolean value
                cadd_type(T_BOOL);
                break;

            case T_STRING: // string literal
                cadd_type(T_STRING);
                break;

            case T_IDF: // identifier
                cadd_var(lreader_n_data().var);
                break;
            
            case T_FISRT_FCALL: // void function call
                lreader_next();
                lreader_change_type(T_FUNC_CALL);
                lreader_remove_next();
                cadd_void_func(lreader_n_data().func);
                G_control.last = false;
                break;

            case T_FUNC_CALL: // function call inside an expression
                lreader_next();
                cadd_func_call(lreader_n_data().func);
                break;
            
            case T_FUNC:
                break;
            
            case T_OP: // operator
                cadd_op(lreader_n_data().op);
                continue;
            
            case T_PARAM_END:
                break;
            
            case T_FCALL_END: // end of function call
                cadd_fcall_end();
                break;
            
            case T_ASSGN_END: // end of assignment
                lreader_next();
                cadd_assgn(lreader_n_data().var);
                break;
            
            case T_ADDRESS_ASSGN: // code generated by the constant propagation, the code is skiped, already must be correct
                lreader_next();
                lreader_next();
                lreader_next();
                lreader_next();
                break;

            default:
                break;
        }

        lreader_next();
    } while (llist_is_not_last()); // read until the end of the list

    dispose_sematic();
}

void cadd_type(uint8_t type)
{
    if (G_control.last)
    {
        // no more constants can be added (function with multiple return values was called previously)
        exit(SEMANTIC_PARAM_COUNT_ERR);
    }
    vu8_push(&G_control.types, type);
    G_control.multi = false;
}

void cadd_var(htab_var_t var)
{
    if (G_control.last)
    {
        // no more variables can be added after a function call with multiple return values
        exit(SEMANTIC_PARAM_COUNT_ERR);
    }
    vu8_push(&G_control.types, get_var_type(var));
    G_control.multi = false;
}

void cadd_func_call(htab_func_t func)
{
    vect_uint8_t ret_vals = get_ret_val_types(func);
    if (ret_vals == NULL || G_control.last)
    {
        // void function cannot be added in an ongoing expression
        // function cannot be added after another function with multiple return types
        exit(SEMANTIC_RET_COUNT_ERR);
    }

    // multile parameters are possible i.e. multiple assignment, multiple return, multiple parameters call
    if (G_control.multi)
    {
        for (int64_t i = 0; i <= vu8_pos(ret_vals); i++)
        {
            vu8_push(&G_control.types, vu8_get_pos(ret_vals, i));
        }

        if (vu8_pos(ret_vals) > 0)
        {

            if (vfht_pos(G_control.functions) >= 0) // nested function calls
            {
                vect_htab_var_t params = get_params(G_control.func);
                // number of return values of one function must equal the number of parameters of the other function
                if (params == NULL || vu8_pos(ret_vals) != vhtv_pos(params)) 
                {
                    // wrong number of function parameters
                    exit(SEMANTIC_PARAM_COUNT_ERR);
                }
            }
            else if (G_control.ret) // function call in return statement
            {
                vect_uint8_t ret_val_types = get_ret_val_types(G_control.defined_func);
                // return values of called function bust equal to the return values of defined function
                if (ret_val_types == NULL || vu8_pos(ret_vals) != vu8_pos(ret_val_types))
                {
                    // wrong number of function return values
                    exit(SEMANTIC_RET_COUNT_ERR);
                }
            }
            else
            {
                G_control.func_multi = true;
            }
        }
    }
    else if (vu8_pos(ret_vals) == 0) // one return value 
    {
        vu8_push(&G_control.types, vu8_get_last(ret_vals));
    }
    else
    {
        exit(SEMANTIC_RET_COUNT_ERR); // function with multiple return values in 1 value context
    }

    vfht_push(&G_control.functions, G_control.func);
    G_control.func = func;
    G_control.multi = true;
    vu8_push(&G_control.types, PARAM_BLOCK); // function parameters start handle addition
}

void cadd_void_func(htab_func_t func)
{
    if (get_ret_val_types(func) != NULL)
    {
        exit(SEMANTIC_RET_COUNT_ERR); // function call without assigning the return value
    }
    vfht_push(&G_control.functions, G_control.func);
    G_control.func = func;
    G_control.multi = true;
    vu8_push(&G_control.types, PARAM_BLOCK); // start of function parameters handle
}

void cadd_op(uint8_t op)
{
    uint8_t type;
    if (vu8_pos(G_control.types) >= 1) // two or more operands
    {
        switch(op)
        {
            case AND:
            case OR:
                type = vu8_pop(G_control.types);
                // type missmatch or not boolean value 
                if (vu8_get_last(G_control.types) != type || type != T_BOOL) // not popping here
                {
                    exit(SEMANTIC_EXPR_ERR);
                }
                // the value remains boolean
                break;
            
            case EQ:
            case NEQ:
                type = vu8_pop(G_control.types);
                // type missmatch
                if (vu8_pop(G_control.types) != type)
                {
                    exit(SEMANTIC_EXPR_ERR);
                }
                vu8_push(&G_control.types, T_BOOL); // adding boolean type as a result
                break;

            case LT:
            case GT:
            case LTE:
            case GTE:
                type = vu8_pop(G_control.types);
                // type missmatch or boolean value 
                if (vu8_pop(G_control.types) != type || type == T_BOOL)
                {
                    exit(SEMANTIC_EXPR_ERR);
                }
                vu8_push(&G_control.types, T_BOOL); // boolean value added as a result
                break;
            
            case ADD:
                type = vu8_pop(G_control.types);
                // type missmatch or boolean value 
                if (vu8_get_last(G_control.types) != type || type == T_BOOL)
                {
                    exit(SEMANTIC_EXPR_ERR);
                }
                else if (type == T_STRING) // change of ADD operator to CONCAT on two strings
                {
                    lreader_n_replace(T_OP, (token_data_t){.op = CONCAT});
                }
                break;
            
            case NEG:
                type = vu8_get_last(G_control.types);               
                if (type == T_INT)
                {
                    lreader_insert_construct(T_INT, (token_data_t){.integer = 0});
                    lreader_n_replace(T_OP, (token_data_t){.op = SUB});
                }
                else if (type == T_FLOAT)
                {
                    lreader_insert_construct(T_FLOAT, (token_data_t){.float64 = 0.0});
                    lreader_n_replace(T_OP, (token_data_t){.op = SUB});
                }
                else
                {
                    exit(SEMANTIC_EXPR_ERR);    
                }
                break;
            
            case POS:
                // values must be integer or float
                if (vu8_get_last(G_control.types) != T_INT && vu8_get_last(G_control.types) != T_FLOAT)
                {
                    exit(SEMANTIC_EXPR_ERR);    
                }
                lreader_remove_next();
                return;
            
            case POS_NOT: 
                // value must be boolean
                if (vu8_get_last(G_control.types) != T_BOOL)
                {
                    exit(SEMANTIC_EXPR_ERR);
                }
                lreader_remove_next();
                return;

            case NOT:
                if (vu8_get_last(G_control.types) != T_BOOL)
                {
                    exit(SEMANTIC_EXPR_ERR);
                }
                break;
            
            case DIV:
                type = vu8_pop(G_control.types);
                // type missmatch, boolean value or string value
                if (vu8_get_last(G_control.types) != type || type == T_BOOL || type == T_STRING)
                {
                    exit(SEMANTIC_EXPR_ERR);
                }
                else if (type == T_INT) // DIV operator replaced with IDIV in case of integer division
                {
                    lreader_n_replace(T_OP, (token_data_t){.op = IDIV});
                }
                break;
            
            case ZERO:
                //simulating negation by adding the corrent zero constant
                type = lreader_find_next_data_type();
                if (type == T_INT)
                {
                    lreader_change_n_data((token_data_t){.integer = 0});
                    lreader_change_n_type(T_INT);
                    vu8_push(&G_control.types, T_INT);
                }
                else if (type == T_FLOAT)
                {
                    lreader_change_n_data((token_data_t){.float64 = 0.0});
                    lreader_change_n_type(T_FLOAT);
                    vu8_push(&G_control.types, T_FLOAT);
                }
                else
                {
                    exit(SEMANTIC_EXPR_ERR);
                }
                break;
            
            default: // MUL and SUB
                type = vu8_pop(G_control.types);
                // type missmatch, boolean value or string value
                if (vu8_get_last(G_control.types) != type || type == T_BOOL || type == T_STRING)
                {
                    print_list();
                    exit(SEMANTIC_EXPR_ERR);
                }
                break;
        }
    }
    else // only one operand
    {
        switch (op)
        {
            case NEG:
                type = vu8_get_last(G_control.types);
                // inserting the correct zero constant infront of the operand and canging the operator to SUB
                if (type == T_INT)
                {
                    lreader_insert_construct(T_INT, (token_data_t){.integer = 0});
                    lreader_n_replace(T_OP, (token_data_t){.op = SUB});
                }
                else if (type == T_FLOAT)
                {
                    lreader_insert_construct(T_FLOAT, (token_data_t){.float64 = 0.0});
                    lreader_n_replace(T_OP, (token_data_t){.op = SUB});
                }
                else
                {
                    exit(SEMANTIC_EXPR_ERR);    
                }
                break;
            
            case POS:
                // type check
                if (vu8_get_last(G_control.types) != T_INT && vu8_get_last(G_control.types) != T_FLOAT)
                {
                    exit(SEMANTIC_EXPR_ERR);    
                }
                lreader_remove_next();
                return;
            
            case POS_NOT:
                // type check
                if (vu8_get_last(G_control.types) != T_BOOL)
                {
                    exit(SEMANTIC_EXPR_ERR);
                }
                lreader_remove_next();
                return;

            case NOT:
                // type check
                if (vu8_get_last(G_control.types) != T_BOOL)
                {
                    exit(SEMANTIC_EXPR_ERR);
                }
                break;
            
            case ZERO:
                //symulating negation by adding the corrent zero constant
                type = lreader_find_next_data_type();
                if (type == T_INT)
                {
                    lreader_change_n_data((token_data_t){.integer = 0});
                    lreader_change_n_type(T_INT);
                    vu8_push(&G_control.types, T_INT);
                    
                }
                else if (type == T_FLOAT)
                {
                    lreader_change_n_data((token_data_t){.float64 = 0.0});
                    lreader_change_n_type(T_FLOAT);
                    vu8_push(&G_control.types, T_FLOAT);
                }
                else
                {
                    exit(SEMANTIC_EXPR_ERR);
                }
                break;
            
            case OR: // or on not generated false value
                if (vu8_get_last(G_control.types) != T_BOOL)
                {
                    exit(SEMANTIC_EXPR_ERR);
                }
                lreader_remove_next();
                return;

            default:
                exit(INTERNAL_ERR);
        }
    }
    lreader_next();
}

void cadd_assgn(htab_var_t var)
{
    if (vu8_pos(G_control.types) < 0)
    {
        // not enought values to be assigned in multi assign expression
        if (G_control.func_multi)
        {
            // function call with not enought return values
            exit(SEMANTIC_RET_COUNT_ERR);
        }

        // wrong number of variables
        exit(SEMANTIC_OTHER_ERR);
    }
    if (get_var_type(var) == 0 || get_var_type(var) == vu8_get_last(G_control.types) || is_special_var(var))
    {
        // the assigned variable either has no type or the type matches with the assigned valu
        // the assigned variable is '_' which has no type
        set_var_type(var, vu8_pop(G_control.types));
    }
    else
    {
        // wrong assigned type
        exit(SEMANTIC_EXPR_ERR);
    }
}

void cadd_multi_assgn_start()
{
    // possible multi value context
    G_control.multi = true;
}

void cadd_multi_assgn_end()
{
    if (vu8_pos(G_control.types) >= 0)
    {
        if (G_control.func_multi)
        {
            // function call with too many return values
            exit(SEMANTIC_RET_COUNT_ERR);
        }

        // too many variables assigned in a multi assigned expression
        exit(SEMANTIC_OTHER_ERR);
    }
    G_control.last = false;
    G_control.multi = false;
    G_control.ret = false;
    G_control.func_multi = false;
}

void cadd_fcall_end()
{
    // special case with print
    if (is_build_in_function(G_control.func) == T_PRINT)
    {
        // all parameter types must be coorect, as print takes any
        while (vu8_pop(G_control.types) != PARAM_BLOCK); // while without body

        G_control.func = vfht_pop(G_control.functions);
        return;
    }

    vect_htab_var_t params = get_params(G_control.func);
    if (params == NULL)
    {
        if (vu8_pop(G_control.types) != PARAM_BLOCK)
        {
            exit(SEMANTIC_PARAM_COUNT_ERR); //function has no parameters, but was called with some
        }
    }
    else if (params != NULL)
    {
        for (int64_t pos = vhtv_pos(params); pos >= 0; pos--)
        {
            if (get_var_type(vhtv_get_pos(params, pos)) != vu8_pop(G_control.types))
            {
                exit(SEMANTIC_PARAM_COUNT_ERR); // the function parameter type does not equal the passed value,
                                                // or the number of passed parameters is lower than expected
            }
        }

        if (vu8_pop(G_control.types) != PARAM_BLOCK)
        {
            exit(SEMANTIC_PARAM_COUNT_ERR); // the number of passed parameters is higher than expected
        }
    }

    G_control.last = false;

    vect_uint8_t ret_vals = get_ret_val_types(G_control.func);
    if (ret_vals == NULL)
    {
        G_control.last = true; // expression must end after a void function call
    }
    else if (vu8_pos(ret_vals) > 0)
    {
        G_control.last = true; // expression must end after a function call with multiple return values
    }

    G_control.multi = false;
    G_control.func = vfht_pop(G_control.functions);
}

void cadd_return_end()
{
    if (get_ret_vals(G_control.defined_func) != NULL)
    {
        if (vu8_pos(G_control.types) >= 0)
        {
            exit(SEMANTIC_RET_COUNT_ERR); // the function has named parameters but still returns value/values
        }
    }
    else
    {
        vect_uint8_t ret_types = get_ret_val_types(G_control.defined_func);
        if (ret_types == NULL && vu8_pos(G_control.types) >= 0)
        {
            exit(SEMANTIC_RET_COUNT_ERR); // the function is void, but returns value/values
        }
        else if (ret_types != NULL)
        {
            int64_t pos = vu8_pos(get_ret_val_types(G_control.defined_func));
            if (pos != vu8_pos(G_control.types))
            {
                exit(SEMANTIC_RET_COUNT_ERR); // number of returned values doesnt match the function return values
            }

            while (pos >= 0)
            {
                if (vu8_get_pos(get_ret_val_types(G_control.defined_func), pos--) != vu8_pop(G_control.types))
                {
                    exit(SEMANTIC_RET_COUNT_ERR); // function return value/s does not match the actual returned value/s
                }
            }
        }
    }

    G_control.last = false;
    G_control.multi = false;
    G_control.ret = false;
}

void cadd_cnd_end()
{
    if (vu8_pos(G_control.types) > 0)
    {
        exit(SEMANTIC_OTHER_ERR); // well, should not happend 
    }
    else if (vu8_pop(G_control.types) != T_BOOL)
    {
        exit(SEMANTIC_EXPR_ERR); // not a boolean value in a condition
    }
}
