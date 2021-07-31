
//=========================================================================================================
// File:        optimizer.c
// Case:        VUT, FIT, IFJ, compiler project
// Date:        9. 12. 2020
// Author(s):   David Mihola
// Contac(s):   xmihol00@stud.fit.vutbr.cz
// Compiled:    gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)
// Description: File containing definitions of functions resonsible for the intermidiate code optimalizations.
//==========================================================================================================

#include "optimizer_private.h"

struct 
{
    htab_var_t assigned_var;
    htab_var_t saved_var;
    uint32_t svar_count;
    htab_func_t defined_func;
    bool recursive_function;
    string_t predef_vars;
    vect_token_t token_stack;
    vect_htab_var_t assigned_vars;
    vect_htab_var_t optimized_vars;
}G_optimizer = {0,};

void init_optimizer()
{
    G_optimizer.token_stack = vect_token_const();
    G_optimizer.assigned_vars = vect_htab_var_const();
    G_optimizer.predef_vars = string_const();
    G_optimizer.optimized_vars = vect_htab_var_const();
}

void dispose_optimizer()
{
    discard_token_vector_free(&G_optimizer.token_stack);
    discard_string(&G_optimizer.predef_vars);
    discard_htab_var_vector(&G_optimizer.assigned_vars);
    discard_htab_var_vector(&G_optimizer.optimized_vars);
}

void optimizer()
{
    init_optimizer(); // init the optimizer module
 
    init_register(); // init the register module

    find_recursive_functions(); // determin which functions are recursive

    lreader_pre_head(); // strat reading the token stream
    do
    {
        switch (lreader_n_type())
        {
            case T_FUNC_DEF:
                lreader_next();
                G_optimizer.defined_func = lreader_n_data().func; // save the defined function
                // set the flag flag if the function is recursove or not
                G_optimizer.recursive_function = func_is_recursive(G_optimizer.defined_func);
                if (!G_optimizer.recursive_function)
                {
                    // predefine the function parameters, if it is not recursive
                    define_global_func_params();
                }
                break;

            case T_MULTI_ASSGN:
                // try to optimize multi assign
                try_multi_assgn_opt();
                break;

            case T_ELIF:
            case T_IF:
                // try to ptimize condition statement
                try_cnd_opt(T_IF_BODY);
                continue;

            case T_FOR2:
                // try to ptimize condition statement
                try_cnd_opt(T_FOR_BODY);
                continue;

            case T_IF_BODY_END:
            case T_ELIF_BODY_END:
                break;

            case T_ELSE_BODY_END:
                break;

            case T_FOR3:
            case T_ASSGN:
                // try to ptimize assign
                try_assgn_opt();
                continue;
            
            case T_FOR1_EMPTY:
                break;

            case T_FOR_BODY_END:
                break;

            case T_RETURN:
                optimize_return();
                break;
            
            case T_FUNC_BODY_END:
                break;
            
            case T_VAR_DEF:
                if (!G_optimizer.recursive_function && !is_special_var(lreader_nn_data().var))
                {
                    // if the function is not recursive store the variable to the header, where it is going to be defined
                    string_substr_push(&G_optimizer.predef_vars, "DEFVAR GF@", 10);
                    string_substr_push(&G_optimizer.predef_vars, 
                                        read_var_name(lreader_nn_data().var), strlen(read_var_name(lreader_nn_data().var)));
                    string_char_push(&G_optimizer.predef_vars, '\n');
                    // set the variable frame to GF
                    set_var_frame(lreader_nn_data().var, true);
                    lreader_remove_next_two(); // remove the tokens
                }
                else
                {
                    // otherwise skip the tokens, the definition will be generated in generator
                    lreader_next();
                    lreader_next();
                }
                continue;
            
            case T_FISRT_FCALL:
            case T_FUNC_CALL:
                lreader_next();
                // funtion call optimization
                optimize_func_call();
                continue;
            
            case T_ADDRESS_ASSGN:
                // generated tokens by the constant propagation
                lreader_next();
                lreader_next();
                lreader_next();
                lreader_next();
                break;

            default:
                break;
        }

        lreader_next();
    } while (llist_is_not_last());

    define_used_registers(&G_optimizer.predef_vars);
}

void define_global_func_params()
{
    vect_htab_var_t vars = get_params(lreader_n_data().func);
    if (vars != NULL)
    {
        // predefine the function parameters to the header
        for (int64_t i = vhtv_pos(vars); i >= 0; i--)
        {
            string_substr_push(&G_optimizer.predef_vars, "DEFVAR GF@", 10);
            string_substr_push(&G_optimizer.predef_vars, read_var_name(vhtv_get_offset(vars, i)), 
                                                         strlen(read_var_name(vhtv_get_offset(vars, i))));
            // set the variable frame to GF
            set_var_frame(vhtv_get_offset(vars, i), true);
            string_char_push(&G_optimizer.predef_vars, '\n');
        }
    }
    
    vars = get_ret_vals(lreader_n_data().func);
    if (vars != NULL)
    {
        // predefine the function named return values to the header
        for (int64_t i = vhtv_pos(vars); i >= 0; i--)
        {
            string_substr_push(&G_optimizer.predef_vars, "DEFVAR GF@", 10);
            string_substr_push(&G_optimizer.predef_vars, read_var_name(vhtv_get_offset(vars, i)), 
                                                         strlen(read_var_name(vhtv_get_offset(vars, i))));
            // set the variable frame to GF
            set_var_frame(vhtv_get_offset(vars, i), true);
            string_char_push(&G_optimizer.predef_vars, '\n');
        }
    }
}

void optimize_condition(uint8_t end_type)
{
    uint32_t count = 0;
    lreader_save_pos(); // save the current posiiton of the reader
    char *reg = reg_get_indirect(); // calculation is not assigned to a specific variable
    lsaved_pos_insert_construct(T_ADDRESS_ASSGN, (token_data_t){0,});
    do
    {
        switch (lreader_n_type())
        {
            case T_IDF:            
            case T_STRING:
            case T_INT:
            case T_FLOAT:
            case T_BOOL:
                // push tokens (operands)
                vtn_push(&G_optimizer.token_stack, tcopy_token(lreader_n_token()));
                count++;
                break;
            
            case T_OP:
                if (tget_token_type(vtn_get_last(G_optimizer.token_stack)) == T_REGISTER)
                {
                    reg_set_emtpy(); //if the first to be popped operand is a register
                }

                // if the operator has two operands (NEG is preplaced for SUB in semantic analysis)
                if (lreader_n_data().op != NOT)
                {
                    if (tget_token_type(vtn_get_2nd_last(G_optimizer.token_stack)) == T_REGISTER)
                    {
                        reg_set_emtpy(); //if the second to be popped operand is a register
                    }
                }
                else
                {
                    count++; // increase by one to ignore the second adress
                }

                if (count > 2) // not and end of the expression
                {
                    if (lreader_n_data().op == NOT) 
                    {
                        // insert instruction
                        lsaved_pos_insert_construct(T_OP, (token_data_t){.op = NOT});
                        // insert destination
                        lsaved_pos_insert_construct(T_REGISTER, (token_data_t){.string = reg_get()});
                        // insert source
                        lsaved_pos_insert(vtn_pop(G_optimizer.token_stack));
                        // replace destination with source on the stack
                        vtn_push(&G_optimizer.token_stack, tcreate_token(T_REGISTER, (token_data_t){.string = reg_get_last()}));
                    }
                    else
                    {
                        // insert instruction
                        lsaved_pos_insert_construct(T_OP, lreader_n_data());
                        // insert destination (register in this case)
                        lsaved_pos_insert_construct(T_REGISTER, (token_data_t){.string = reg_get()});
                        // insert 1st source
                        lsaved_pos_insert(vtn_get_2nd_last(G_optimizer.token_stack));
                        // insert 2nd source
                        lsaved_pos_insert(vtn_get_last(G_optimizer.token_stack));
                        // replace the source operands by the destination on the stack
                        vtn_replace_last_2(G_optimizer.token_stack, tcreate_token(T_REGISTER, (token_data_t){.string = reg_get_last()}));

                        if (lreader_n_data().op == GTE || lreader_n_data().op == LTE || lreader_n_data().op == NEQ)
                        {
                            // GTE -> LT and NOT, LTE -> GT and NOT, NEQ -> EQ and NOT
                            lsaved_pos_insert_construct(T_OP, (token_data_t){.op = NOT});
                            // perform the calculation to the last used register
                            lsaved_pos_insert_construct(T_REGISTER, (token_data_t){.string = reg_get_last()});
                            lsaved_pos_insert_construct(T_REGISTER, (token_data_t){.string = reg_get_last()});
                        }
                    }
                }
                else
                {
                    if (lreader_nn_type() == end_type) // end of an expression
                    {
                        if (lreader_n_data().op == EQ) // dont caluclate the value, jump based on the comparison
                        {
                            // jumping the the value is false, therfore the values are not equal
                            if (end_type == T_IF_BODY)
                            {
                                lsaved_pos_insert_construct(T_JMP_IF_NEQ, (token_data_t){0,});    
                            }
                            else
                            {
                                lsaved_pos_insert_construct(T_JMP_FOR_NEQ, (token_data_t){0,});
                            }
                        }
                        else if (lreader_n_data().op == NEQ) // dont caluclate the value, jump based on the comparison
                        {
                            if (end_type == T_IF_BODY)
                            {
                                lsaved_pos_insert_construct(T_JMP_IF_EQ, (token_data_t){0,});
                            }
                            else
                            {
                                lsaved_pos_insert_construct(T_JMP_FOR_EQ, (token_data_t){0,});
                            }
                        }
                        else if (lreader_n_data().op == LTE)
                        {
                            // calcualte GT
                            lsaved_pos_insert_construct(T_OP, (token_data_t){.op = GT});
                            lsaved_pos_insert_construct(T_REGISTER, (token_data_t){.string = reg});
                            lsaved_pos_insert(vtn_get_2nd_last(G_optimizer.token_stack));
                            lsaved_pos_insert(vtn_get_last(G_optimizer.token_stack));
                            vtn_replace_last_2(G_optimizer.token_stack, tcreate_token(T_REGISTER, (token_data_t){.string = reg}));
                            // jump in reverse
                            if (end_type == T_IF_BODY) 
                            {
                                lsaved_pos_insert_construct(T_JMP_IF_NEQ, (token_data_t){0,});
                            }
                            else
                            {
                                lsaved_pos_insert_construct(T_JMP_FOR_NEQ, (token_data_t){0,});
                            }
                            vtn_push(&G_optimizer.token_stack, tcreate_token(T_BOOL, (token_data_t){.boolean = false}));
                        }
                        else if (lreader_n_data().op == GTE)
                        {
                            // calcualte LT
                            lsaved_pos_insert_construct(T_OP, (token_data_t){.op = LT});
                            lsaved_pos_insert_construct(T_REGISTER, (token_data_t){.string = reg});
                            lsaved_pos_insert(vtn_get_2nd_last(G_optimizer.token_stack));
                            lsaved_pos_insert(vtn_get_last(G_optimizer.token_stack));
                            vtn_replace_last_2(G_optimizer.token_stack, tcreate_token(T_REGISTER, (token_data_t){.string = reg}));
                            // jump in reverse
                            if (end_type == T_IF_BODY)
                            {
                                lsaved_pos_insert_construct(T_JMP_IF_NEQ, (token_data_t){0,});
                            }
                            else
                            {
                                lsaved_pos_insert_construct(T_JMP_FOR_NEQ, (token_data_t){0,});
                            }
                            vtn_push(&G_optimizer.token_stack, tcreate_token(T_BOOL, (token_data_t){.boolean = false}));
                        }
                        else if (lreader_n_data().op == NOT)
                        {
                            // calculate NOT
                            lsaved_pos_insert_construct(T_OP, (token_data_t){.op = NOT});
                            lsaved_pos_insert_construct(T_REGISTER, (token_data_t){.string = reg});
                            lsaved_pos_insert(vtn_pop(G_optimizer.token_stack));
                            vtn_push(&G_optimizer.token_stack, tcreate_token(T_REGISTER, (token_data_t){.string = reg}));
                            if (end_type == T_IF_BODY)
                            {
                                lsaved_pos_insert_construct(T_JMP_IF_NEQ, (token_data_t){0,});
                            }
                            else
                            {
                                lsaved_pos_insert_construct(T_JMP_FOR_NEQ, (token_data_t){0,});
                            }
                            vtn_push(&G_optimizer.token_stack, tcreate_token(T_BOOL, (token_data_t){.boolean = true}));
                        }
                        else
                        {
                            // calculate GT or LT
                            lsaved_pos_insert_construct(T_OP, lreader_n_data());
                            lsaved_pos_insert_construct(T_REGISTER, (token_data_t){.string = reg});
                            lsaved_pos_insert(vtn_get_2nd_last(G_optimizer.token_stack));
                            lsaved_pos_insert(vtn_get_last(G_optimizer.token_stack));
                            vtn_replace_last_2(G_optimizer.token_stack, tcreate_token(T_REGISTER, (token_data_t){.string = reg}));
                            if (end_type == T_IF_BODY)
                            {
                                lsaved_pos_insert_construct(T_JMP_IF_NEQ, (token_data_t){0,});
                            }
                            else
                            {
                                lsaved_pos_insert_construct(T_JMP_FOR_NEQ, (token_data_t){0,});
                            }
                            // jump based on a comparison with true value
                            vtn_push(&G_optimizer.token_stack, tcreate_token(T_BOOL, (token_data_t){.boolean = true}));
                        }
                        // insert the operands
                        lsaved_pos_insert(vtn_get_2nd_last(G_optimizer.token_stack));
                        lsaved_pos_insert(vtn_get_last(G_optimizer.token_stack));
                        vtn_reset(G_optimizer.token_stack);
                        count--;
                    }
                    else
                    {
                        if (lreader_n_data().op == NOT)
                        {
                            // calculate NOT to the register assigned for indirect calculation
                            lsaved_pos_insert_construct(T_OP, (token_data_t){.op = NOT});
                            lsaved_pos_insert_construct(T_REGISTER, (token_data_t){.string = reg});
                            lsaved_pos_insert(vtn_pop(G_optimizer.token_stack));
                            vtn_push(&G_optimizer.token_stack, tcreate_token(T_REGISTER, (token_data_t){.string = reg}));
                        }
                        else
                        {
                            // calculate other operators to the register assigned for indirect calculation
                            lsaved_pos_insert_construct(T_OP, lreader_n_data());
                            lsaved_pos_insert_construct(T_REGISTER, (token_data_t){.string = reg});
                            lsaved_pos_insert(vtn_get_2nd_last(G_optimizer.token_stack));
                            lsaved_pos_insert(vtn_get_last(G_optimizer.token_stack));
                            vtn_replace_last_2(G_optimizer.token_stack, tcreate_token(T_REGISTER, (token_data_t){.string = reg}));
                            if (lreader_n_data().op == GTE || lreader_n_data().op == LTE || lreader_n_data().op == NEQ)
                            {
                                lsaved_pos_insert_construct(T_OP, (token_data_t){.op = NOT});
                                lsaved_pos_insert_construct(T_REGISTER, (token_data_t){.string = reg});
                                lsaved_pos_insert_construct(T_REGISTER, (token_data_t){.string = reg});
                            }
                        }
                    }
                }
                count--; // decrease the number of operands
                break;
        }

        lreader_next();
    } while (lreader_n_type() != end_type);

    if (count == 1) // only one read operand
    {
        if (end_type == T_IF_BODY)
        {
            // generate jump based on a signle value
            lsaved_pos_insert_construct(T_SINGLE_VAL_IF, (token_data_t){0,});
        }
        else
        {
            lsaved_pos_insert_construct(T_SINGLE_VAL_FOR, (token_data_t){0,});
        }
        lsaved_pos_insert(vtn_pop(G_optimizer.token_stack));
    }

    reg_reset(); // reset used registers
    lreader_next();
    lsaved_remove_until(end_type); // remove the converted part of the list
}

void optimize_adress_assign(uint8_t end_type)
{
    uint32_t count = 0;
    bool move = true;
    char *reg = reg_get_indirect();

    do
    {
        switch (lreader_n_type())
        {
            case T_IDF:            
            case T_STRING:
            case T_INT:
            case T_FLOAT:
            case T_BOOL:
                vtn_push(&G_optimizer.token_stack, tcopy_token(lreader_n_token()));
                count++;
                break;
            
            case T_OP:
                move = false;

                if (lreader_n_data().op != NOT)
                {
                    if (tget_token_type(vtn_get_2nd_last(G_optimizer.token_stack)) == T_REGISTER)
                    {
                        reg_set_emtpy();
                    }
                }
                else
                {
                    count++;
                }
                if (tget_token_type(vtn_get_last(G_optimizer.token_stack)) == T_REGISTER)
                {
                    reg_set_emtpy();
                }
                
                lsaved_pos_insert_construct(T_OP, lreader_n_data());
                if (count > 2) // acquire new register
                {
                    if (lreader_n_data().op == NOT)
                    {
                        lsaved_pos_insert_construct(T_REGISTER, (token_data_t){.string = reg_get()});
                        lsaved_pos_insert(vtn_pop(G_optimizer.token_stack));
                        vtn_push(&G_optimizer.token_stack, tcreate_token(T_REGISTER, (token_data_t){.string = reg_get_last()}));
                    }
                    else
                    {
                        lsaved_pos_insert_construct(T_REGISTER, (token_data_t){.string = reg_get()});
                        lsaved_pos_insert(vtn_get_2nd_last(G_optimizer.token_stack));
                        lsaved_pos_insert(vtn_get_last(G_optimizer.token_stack));
                        vtn_replace_last_2(G_optimizer.token_stack, tcreate_token(T_REGISTER, (token_data_t){.string = reg_get_last()}));
                        if (lreader_n_data().op == GTE || lreader_n_data().op == LTE || lreader_n_data().op == NEQ)
                        {
                            lsaved_pos_insert_construct(T_OP, (token_data_t){.op = NOT});
                            lsaved_pos_insert_construct(T_REGISTER, (token_data_t){.string = reg_get_last()});
                            lsaved_pos_insert_construct(T_REGISTER, (token_data_t){.string = reg_get_last()});
                        }
                    }
                }
                else
                {
                    if (lreader_nn_type() == end_type) // end of an expression
                    {
                        if (lreader_n_data().op == NOT)
                        {
                            // perform NOT to the assigned variable
                            lsaved_pos_insert_construct(T_IDF, (token_data_t){.var = G_optimizer.assigned_var});
                            lsaved_pos_insert(vtn_pop(G_optimizer.token_stack));
                        }
                        else
                        {
                            // perform the last instrunction to the assigned variable
                            lsaved_pos_insert_construct(T_IDF, (token_data_t){.var = G_optimizer.assigned_var});
                            lsaved_pos_insert(vtn_get_2nd_last(G_optimizer.token_stack));
                            lsaved_pos_insert(vtn_get_last(G_optimizer.token_stack));
                            vtn_reset(G_optimizer.token_stack);
                            if (lreader_n_data().op == GTE || lreader_n_data().op == LTE || lreader_n_data().op == NEQ)
                            {
                                // negate the result in case of GTE, LTE and NEQ
                                lsaved_pos_insert_construct(T_OP, (token_data_t){.op = NOT});
                                lsaved_pos_insert_construct(T_IDF, (token_data_t){.var = G_optimizer.assigned_var});
                                lsaved_pos_insert_construct(T_IDF, (token_data_t){.var = G_optimizer.assigned_var});
                            }
                        }
                    }
                    else // use the indirect register
                    {
                        if (lreader_n_data().op == NOT)
                        {
                            lsaved_pos_insert_construct(T_REGISTER, (token_data_t){.string = reg});
                            lsaved_pos_insert(vtn_pop(G_optimizer.token_stack));
                            vtn_push(&G_optimizer.token_stack, tcreate_token(T_REGISTER, (token_data_t){.string = reg}));
                        }
                        else
                        {
                            lsaved_pos_insert_construct(T_REGISTER, (token_data_t){.string = reg});
                            lsaved_pos_insert(vtn_get_2nd_last(G_optimizer.token_stack));
                            lsaved_pos_insert(vtn_get_last(G_optimizer.token_stack));
                            vtn_replace_last_2(G_optimizer.token_stack, tcreate_token(T_REGISTER, (token_data_t){.string = reg}));
                            if (lreader_n_data().op == GTE || lreader_n_data().op == LTE || lreader_n_data().op == NEQ)
                            {
                                lsaved_pos_insert_construct(T_OP, (token_data_t){.op = NOT});
                                lsaved_pos_insert_construct(T_REGISTER, (token_data_t){.string = reg});
                                lsaved_pos_insert_construct(T_REGISTER, (token_data_t){.string = reg});
                            }
                        }
                    }
                }
                count--; // decrease the number of operands on the stack
                break;
        }

        lreader_next();
    } while (lreader_n_type() != end_type);
    lreader_next();
    
    if (move) // just assign of a variable
    {
        // insert assign tokens
        lsaved_pos_insert_construct(T_MOV, (token_data_t){});
        lsaved_pos_insert_construct(T_IDF, (token_data_t){.var = G_optimizer.assigned_var});
        lsaved_pos_insert(vtn_pop(G_optimizer.token_stack));
    }

    reg_reset(); // reset all used registers
    lsaved_remove_until(end_type); // remove converted part of the list
}

void optimize_adress_assign_to_partial() // load the result of the optimization to a partial linked list
{
    uint32_t count = 0;
    bool move = true;
    char *reg = reg_get_indirect();

    do
    {
        // same as optimize adress assgn, apart from that the expression is saved to the partial list
        switch (lreader_n_type())
        {
            case T_IDF:            
            case T_STRING:
            case T_INT:
            case T_FLOAT:
            case T_BOOL:
                vtn_push(&G_optimizer.token_stack, tcopy_token(lreader_n_token()));
                count++;
                break;
            
            case T_OP:
                move = false;

                if (lreader_n_data().op != NOT)
                {
                    if (tget_token_type(vtn_get_2nd_last(G_optimizer.token_stack)) == T_REGISTER)
                    {
                        reg_set_emtpy();
                    }
                }
                else
                {
                    count++;
                }
                if (tget_token_type(vtn_get_last(G_optimizer.token_stack)) == T_REGISTER)
                {
                    reg_set_emtpy();
                }
                
                lpartial_insert_create(T_OP, lreader_n_data()); // insert the instruction
                if (count > 2)
                {
                    if (lreader_n_data().op == NOT)
                    {
                        lpartial_insert_create(T_REGISTER, (token_data_t){.string = reg_get()});
                        lpartial_insert(vtn_pop(G_optimizer.token_stack));
                        vtn_push(&G_optimizer.token_stack, tcreate_token(T_REGISTER, (token_data_t){.string = reg_get_last()}));
                    }
                    else
                    {
                        lpartial_insert_create(T_REGISTER, (token_data_t){.string = reg_get()});
                        lpartial_insert(vtn_get_2nd_last(G_optimizer.token_stack));
                        lpartial_insert(vtn_get_last(G_optimizer.token_stack));
                        vtn_replace_last_2(G_optimizer.token_stack, tcreate_token(T_REGISTER, (token_data_t){.string = reg_get_last()}));
                        if (lreader_n_data().op == GTE || lreader_n_data().op == LTE || lreader_n_data().op == NEQ)
                        {
                            lpartial_insert_create(T_OP, (token_data_t){.op = NOT});
                            lpartial_insert_create(T_REGISTER, (token_data_t){.string = reg_get_last()});
                            lpartial_insert_create(T_REGISTER, (token_data_t){.string = reg_get_last()});
                        }
                    }
                }
                else
                {
                    if (lreader_nn_type() == T_PARAM_END)
                    {
                        if (lreader_n_data().op == NOT)
                        {
                            lpartial_insert_create(T_IDF, (token_data_t){.var = G_optimizer.assigned_var});
                            lpartial_insert(vtn_pop(G_optimizer.token_stack));
                        }
                        else
                        {
                            lpartial_insert_create(T_IDF, (token_data_t){.var = G_optimizer.assigned_var});
                            lpartial_insert(vtn_get_2nd_last(G_optimizer.token_stack));
                            lpartial_insert(vtn_get_last(G_optimizer.token_stack));
                            vtn_reset(G_optimizer.token_stack);
                            if (lreader_n_data().op == GTE || lreader_n_data().op == LTE || lreader_n_data().op == NEQ)
                            {
                                lpartial_insert_create(T_OP, (token_data_t){.op = NOT});
                                lpartial_insert_create(T_IDF, (token_data_t){.var = G_optimizer.assigned_var});
                                lpartial_insert_create(T_IDF, (token_data_t){.var = G_optimizer.assigned_var});
                            }
                        }
                    }
                    else
                    {
                        if (lreader_n_data().op == NOT)
                        {
                            lpartial_insert_create(T_REGISTER, (token_data_t){.string = reg});
                            lpartial_insert(vtn_pop(G_optimizer.token_stack));
                            vtn_push(&G_optimizer.token_stack, tcreate_token(T_REGISTER, (token_data_t){.string = reg}));
                        }
                        else
                        {
                            lpartial_insert_create(T_REGISTER, (token_data_t){.string = reg});
                            lpartial_insert(vtn_get_2nd_last(G_optimizer.token_stack));
                            lpartial_insert(vtn_get_last(G_optimizer.token_stack));
                            vtn_replace_last_2(G_optimizer.token_stack, tcreate_token(T_REGISTER, (token_data_t){.string = reg}));
                            if (lreader_n_data().op == GTE || lreader_n_data().op == LTE || lreader_n_data().op == NEQ)
                            {
                                lpartial_insert_create(T_OP, (token_data_t){.op = NOT});
                                lpartial_insert_create(T_REGISTER, (token_data_t){.string = reg});
                                lpartial_insert_create(T_REGISTER, (token_data_t){.string = reg});
                            }
                        }
                    }
                }
                count--;
                break;
        }

        lreader_remove_next();
    } while (lreader_n_type() != T_PARAM_END);
    lreader_remove_next();
    
    if (move)
    {
        // simple assign without any expressions
        lpartial_insert_create(T_MOV, (token_data_t){});
        lpartial_insert_create(T_IDF, (token_data_t){.var = G_optimizer.assigned_var});
        lpartial_insert(vtn_pop(G_optimizer.token_stack));
    }

    reg_reset();
}

void try_assgn_opt()
{
    bool last_step = true;
    lreader_save_pos();
    lreader_next();
    lreader_next();
    if (lreader_n_type() == T_VARS_DUMMY) 
    {
        return; // variable is not assigned as it has known value
    }
    G_optimizer.assigned_var = lreader_data().var; // store the assigned variable

    if (lreader_nn_type() == T_FUNC_CALL)
    {
        lreader_next();
        // if the function can be converted sirectly to instruction (input, print...)
        if (is_function_to_instruction(lreader_nn_data().func))
        {
            bool op = false;
            uint32_t level = 1;
            // check that the function call is not followed by operators
            while (lreader_move_next_type() != T_ASSGN_END)
            {
                if (lreader_n_type() == T_FUNC_CALL)
                {
                    level++;
                }
                else if (lreader_n_type() == T_FCALL_END)
                {
                    level--;
                }
                else if (lreader_n_type() == T_OP && level == 0)
                {
                    op = true;
                    break;
                }
            }

            if (op == false)
            {
                // prepare the linked list
                lreader_reaload_saved_pos();
                lreader_remove_next_two();
                lreader_remove_next();
                lreader_next();
                // optimize the built-in function call
                optimize_func_call();
                lreader_remove_until(T_ASSGN_END);
                G_optimizer.assigned_var = NULL;
                return;
            }
            else
            {
                // dont optimize the function call
                lreader_reaload_saved_pos(); // move back at the start of the assignment
                lreader_save_pos();
                lreader_next();
            }
        }
    }

    while (lreader_type() != T_ASSGN_END) // check the assigned expression
    {
        // check for functions
        if (lreader_type() == T_FUNC_CALL)
        {
            // reset the assigned variable
            G_optimizer.assigned_var = NULL;
            // function call optimization
            optimize_func_call();
            // the expression cannot be converted to the TRA
            last_step = false;
            if (lreader_type() == T_PARAM_END)
            {
                break;
            }
        }
        lreader_next();
    }

    if (last_step) // optimization possible
    {
        // prepare the linked list
        lreader_reaload_saved_pos();
        lreader_next();
        // change T_ASSGN to T_ADRESS_ASSGN
        lreader_change_type(T_ADDRESS_ASSGN);
        lreader_save_pos();
        lreader_next();
        // optimize the assign
        optimize_adress_assign(T_ASSGN_END);
    }
    // reset the optimized variable
    G_optimizer.assigned_var = NULL;
}

void try_multi_assgn_opt()
{
    lreader_save_pos();
    lreader_next();

    uint8_t bin_func;

    do
    {
        // load all assigned variables
        vhtv_push(&G_optimizer.assigned_vars, lreader_n_data().var);
        lreader_remove_next();
    } while(lreader_n_type() != T_VARS_END);
    
    
    if (lreader_find_occurence_until(T_NEXT_VAL, T_MULTI_ASSGN_END)) // check if the does not consists only of a function call
    {
        lreader_insert_construct(T_MULTI_ASSGN, (token_data_t){0});
        lreader_remove_next();
        // optimize the multiple assignment
        optimize_multi_assgn();
    }
    else
    {
        lreader_remove_next();
        if (lreader_n_type() == T_FUNC_CALL)
        {
            if ((bin_func = is_input_function(lreader_nn_data().func)))
            {
                // optimize the input function call
                optimize_input(bin_func);
            }
            else
            {
                lreader_next();
                // optimize regular function call with multiple return values
                optimize_func_call();
            }
        }
        vhtv_reset(G_optimizer.assigned_vars);
    }
}

void optimize_multi_assgn()
{
    bool last_step = true;
    do
    {
        lreader_save_pos_to_stack();
        do
        {
            if (lreader_n_type() == T_FUNC_CALL)
            {
                lreader_next();
                // optimize function call used in the expression
                optimize_func_call();
                last_step = false; // decativate the further optimization 
                if (lreader_n_type() == T_NEXT_VAL)
                {
                    break;
                }
            }

        } while (lreader_move_next_type() != T_NEXT_VAL);

        if (last_step == false)
        {
            // variable cannot be assigned as it contains a function call
            vhtv_pop(G_optimizer.assigned_vars);
            lreader_next();
            lreader_pop_stack();
            last_step = true; // set the flag to continue with the next one
        }
        // the assigned variable cannot be used in later parts of the multi assignment
        else if (!lreader_find_var_until(vhtv_get_last(G_optimizer.assigned_vars), T_ASSGN_END))
        {
            // the variable is not used, therfore can be evaluated before the other parts of the multi assignment are evaluated
            lreader_load_pos_from_stack();
            lreader_change_type(T_ADDRESS_ASSGN);
            lreader_save_pos();
            G_optimizer.assigned_var = vhtv_pop(G_optimizer.assigned_vars);
            vhtv_push(&G_optimizer.optimized_vars, G_optimizer.assigned_var);
            // optimize the adress assign
            optimize_adress_assign(T_NEXT_VAL);
            lreader_insert_construct(T_ADR_ASSGN_END, (token_data_t){0,});
            G_optimizer.assigned_var = NULL;
        }
        else
        {
            // varibale cannot be assigned as it is used later in the expression
            lreader_load_pos_from_stack(); 
            lreader_change_type(T_ASSGN);
            lreader_skip_until(T_NEXT_VAL);
            lreader_next();
            vhtv_pop(G_optimizer.assigned_vars);
        }

    } while (vhtv_pos(G_optimizer.assigned_vars) > 0);
    // until the second to last variables of the multiassign expression inst assigned

    lreader_save_pos_to_stack(); // save the current position and optimize the last assigned variable
    do
    {
        if (lreader_n_type() == T_FUNC_CALL)
        {
            lreader_next();
            // optimize function call, if it appears in the expression
            optimize_func_call();
            last_step = false;
        }

    } while (lreader_move_next_type() != T_ASSGN_END);
    
    if (last_step == false) // further optimization cannot be performed
    {
        lreader_save_pos();
        lreader_load_pos_from_stack();
        lreader_change_type(T_ASSGN);
        vhtv_pop(G_optimizer.assigned_vars); 
        last_step = true;
        lreader_reaload_saved_pos();
    }
    else
    {
        // the last variable can be always assigned as it does not interfier with the previous variables
        // set up the list
        lreader_next();
        lreader_insert_construct(T_NEXT_VAL, (token_data_t){0,});
        lreader_load_pos_from_stack();
        lreader_change_type(T_ADDRESS_ASSGN);
        lreader_save_pos();
        // store the optimized variable
        G_optimizer.assigned_var = vhtv_pop(G_optimizer.assigned_vars);
        vhtv_push(&G_optimizer.optimized_vars, G_optimizer.assigned_var);
        // optimize the assigned variable
        optimize_adress_assign(T_NEXT_VAL);
        lreader_change_type(T_ADR_ASSGN_END);
        G_optimizer.assigned_var = NULL;
    }          

    while(vhtv_pos(G_optimizer.optimized_vars) >= 0)
    {
        if (lreader_nn_data().var == vhtv_get_last(G_optimizer.optimized_vars))
        {
            // if the adress assignment was successful, remove the assigned variables from the list
            vhtv_pop(G_optimizer.optimized_vars);
            lreader_remove_next_two();
        }
        else
        {
            // skip the assigned variables
            lreader_next();
            lreader_next();
        }
    }
}

void try_cnd_opt(uint8_t end_type)
{
    bool last_step = 1;
    lreader_save_pos();
    lreader_next();
    
    while(lreader_type() != end_type)
    {
        if (lreader_type() == T_FUNC_CALL)
        {
            // optimize function call
            optimize_func_call();
            last_step = false;
        }

        lreader_next();
    }

    if (last_step) // if the optimized expression does not contain function calls
    {
        lreader_reaload_saved_pos();
        lreader_next();
        optimize_condition(end_type);
    }
}

void optimize_func_call()
{
    bool last_step = true;
    // store the called function
    htab_func_t called_function = lreader_n_data().func;
    vect_htab_var_t params;
    int64_t param_count = 0;
    int64_t ret_vals = 0;

    if (is_function_to_instruction(called_function))
    {
        // remove the function definition
        if (lreader_remove_func_def())
        {
            lreader_change_n_type(T_EMPTY);
            return;
        }
        // set the function parameter count, as it had no parameters
        param_count = -2;
    }
    else
    {
        lreader_next();
        if (lreader_n_type() == T_FCALL_END)
        {
            // function without parameters
            return;
        }
    }

    if (func_is_recursive(called_function))
    {
        // resucursive function
        param_count = vhtv_pos(get_params(called_function)) + 1;
        while (param_count)
        {
            if (lreader_type() == T_FUNC_CALL)
            {
                // optimize function calls used as parameters of the resursive function
                optimize_func_call();
            }

            if (lreader_type() == T_PARAM_END)
            {
                param_count--; // decrease the number of parameters
            }
            lreader_next();
        }
    }
    else // non recursive function
    {
        lpartial_create(); // create new partial linked list
        lreader_remove_next();
        lreader_save_pos_to_stack(); // save the current position
        params = get_params(called_function); // retrieve the function parameters
        do
        {
            last_step = true;
            while (lreader_type() != T_PARAM_END)
            {
                if (lreader_type() == T_FUNC_CALL)
                {
                    last_step = false;
                    ret_vals = vu8_pos(get_ret_val_types(lreader_n_data().func)) + 1;
                    if (G_optimizer.svar_count++ == 0)
                    {
                        G_optimizer.saved_var = G_optimizer.assigned_var;
                    }
                    G_optimizer.assigned_var = NULL;
                    // optimize function call as a parameter
                    optimize_func_call();
                    if (--G_optimizer.svar_count == 0)
                    {
                        G_optimizer.assigned_var = G_optimizer.saved_var;
                    }
                    if (lreader_type() == T_PARAM_END)
                    {
                        break;
                    }
                }
                lreader_next();
            }

            if (last_step == false)
            {
                lreader_pop_stack();
                last_step = true;
                if (is_function_to_instruction(called_function))
                {
                    last_step = optimize_build_in_fcall(called_function, true);
                    if (last_step) // optimization succesful
                    {
                        // insert the partial list to the main list
                        lpartial_insert_partial_to_reader();
                        lreader_change_n_type(T_EMPTY); // dummy token
                        return;
                    }
                }
                else
                {
                    lreader_remove();
                    lreader_change_type(T_EMPTY); // dummy token
                    lreader_save_pos_to_stack();
                    if (ret_vals == 1) // function has 1 return value
                    {
                        // assign the function parameter with the returned value
                        lpartial_insert_create_after(T_IDF, (token_data_t){.var = vhtv_get_pos(params, param_count++)});
                        lpartial_insert_create_after(T_ASSGN_END, (token_data_t){0,});    
                    }
                    else
                    {
                        // function has multiple return values
                        param_count += ret_vals;
                        while(ret_vals > 0)
                        {
                            // assign the parameters with the return values
                            lpartial_insert_create_after(T_IDF, (token_data_t){.var = vhtv_get_offset(params, --ret_vals)});
                            lpartial_insert_create_after(T_ASSGN_END, (token_data_t){0,});    
                        }
                    }
                }
            }
            else // parameter expression can be optimized (directly assgned to the parameter)
            {
                lreader_load_pos_from_stack(); // load correct position in the main list
                if (is_function_to_instruction(called_function)) // the called function is built-in
                {
                    last_step = optimize_build_in_fcall(called_function, false);
                    if (last_step)
                    {
                        lpartial_insert_partial_to_reader();
                        lreader_change_n_type(T_EMPTY);
                        return;
                    }
                }
                else
                {
                    lpartial_insert_create(T_ADDRESS_ASSGN, (token_data_t){0,}); // change the assign generatrion
                    lreader_save_pos();
                    // set the assigned variable
                    G_optimizer.assigned_var = vhtv_get_pos(params, param_count++);
                    // assign the function parameter
                    optimize_adress_assign_to_partial();
                    lreader_reaload_saved_pos();
                    lreader_remove_next(); // remove the T_PARAM_END token
                    lpartial_insert_create(T_ADR_ASSGN_END, (token_data_t){0,});
                    G_optimizer.assigned_var = NULL;
                }
                lreader_save_pos_to_stack();
            }

        } while(param_count <= vhtv_pos(params)); // until there are unasigned parameters

        lreader_pop_stack();
        // function call end
        lpartial_insert_create_last(T_FCALL_END, (token_data_t){0,});
        lpartial_insert_partial_to_reader(); // insert the partial list to the main list
    }
}

bool optimize_build_in_fcall(htab_func_t called_function, bool last_step)
{
    switch(is_function_to_instruction(called_function))
    {
        case T_PRINT:
            if (last_step)
            {
                // value is on stack
                lpartial_insert_create(T_ASSGN_END, (token_data_t){0,});
                // pop to tmp variable
                lpartial_insert_create(T_IDF, (token_data_t){.var = get_tmp_var(0)});
                // print the tmp variable
                lpartial_insert_create(T_WRITE_INST, (token_data_t){0,});
                lpartial_insert_create(T_REGISTER, (token_data_t){.string = "GF@?temp&1"});
                lpartial_insert_create(T_ADR_ASSGN_END, (token_data_t){0,});
            }
            else
            {
                lreader_save_pos();
                if (lreader_nn_type() == T_PARAM_END)
                {
                    // print a single variable
                    lreader_remove();
                    lpartial_insert_create(T_WRITE_INST, (token_data_t){0,});
                    lpartial_insert_create(lreader_type(), lreader_data());
                    lreader_remove();
                    lpartial_insert_create(T_ADR_ASSGN_END, (token_data_t){0,});
                }
                else
                {
                    // print expression
                    lpartial_insert_create(T_ADDRESS_ASSGN, (token_data_t){0,});
                    G_optimizer.assigned_var = get_tmp_var(0);
                    // calculate the expression
                    optimize_adress_assign_to_partial(T_PARAM_END);
                    lreader_reaload_saved_pos();
                    lpartial_insert_create(T_ADR_ASSGN_END, (token_data_t){0,});
                    // print the result
                    lpartial_insert_create(T_WRITE_INST, (token_data_t){0,});
                    lpartial_insert_create(T_REGISTER, (token_data_t){.string = "GF@?temp&1"});
                    lpartial_insert_create(T_ADR_ASSGN_END, (token_data_t){0,});
                    G_optimizer.assigned_var = NULL;
                }
            }
            if (lreader_n_type() == T_FCALL_END)
            {
                lreader_change_type(T_EMPTY);
                return true;
            }
            else
            {
                lreader_change_type(T_EMPTY);
                // insert the partial list to the main list
                lpartial_insert_partial_to_reader();
                lpartial_create();
                if (last_step)
                {
                    lreader_save_pos_to_stack();
                }
                lreader_remove_next();
                return false;
            }
            break;
        
        case T_LEN: //STRLEN
            optimize_func_to_inst(T_LEN_INST, last_step);
            break;
        
        case T_I2F: // INT2FLOAT
            optimize_func_to_inst(T_I2F_INST, last_step);
            break;
        
        case T_F2I: // FLOAT2INT
            optimize_func_to_inst(T_F2I_INST, last_step);
            break;
    }

    if (lreader_n_type() == T_FCALL_END)
    {
        lreader_change_type(T_EMPTY);
        return true;
    }
    else
    {
        lreader_remove();
    }
    return false;
}

void optimize_func_to_inst(uint8_t instruction, bool last_step)
{
    if (last_step) // parameter value is on the stack
    {
        lreader_save_pos_to_stack();
        if (G_optimizer.assigned_var) // assigned varibale is known
        {
            // pop to the tmp_var
            lpartial_insert_create(T_ASSGN_END, (token_data_t){0,}); 
            lpartial_insert_create(T_IDF, (token_data_t){.var = get_tmp_var(0)});
            // add the function instruction
            lpartial_insert_create(instruction, (token_data_t){0,});
            // the addres
            lpartial_insert_create(T_IDF, (token_data_t){.var = G_optimizer.assigned_var});
            // the previously popped value as the source
            lpartial_insert_create(T_REGISTER, (token_data_t){.string = "GF@?temp&1"});
            lpartial_insert_create(T_ADR_ASSGN_END, (token_data_t){0,});
            // function call end
            lreader_change_n_type(T_FCALL_END);
        }
        else // assigned variable is uknown
        {
            if (instruction == T_I2F_INST)
            {
                lpartial_insert_create(T_I2FS_INST, (token_data_t){0,});
            }
            else if (instruction == T_F2I_INST)
            {
                lpartial_insert_create(T_F2IS_INST, (token_data_t){0,});
            }
            else
            {
                lpartial_insert_create(T_ASSGN_END, (token_data_t){0,});
                // pop to a tmp variable
                lpartial_insert_create(T_IDF, (token_data_t){.var = get_tmp_var(0)});
                lpartial_insert_create(instruction, (token_data_t){0,});
                // calculate the result from source to destination
                lpartial_insert_create(T_REGISTER, (token_data_t){.string = "GF@?temp&1"});
                lpartial_insert_create(T_REGISTER, (token_data_t){.string = "GF@?temp&1"});
                // push the destination
                lpartial_insert_create(T_PUSHS, (token_data_t){0,});
                lpartial_insert_create(T_IDF, (token_data_t){.var = get_tmp_var(0)});
                lpartial_insert_create(T_ADR_ASSGN_END, (token_data_t){0,});
            }
        }
    }
    else // parameter value is assigned directly
    {
        if (lreader_nn_type() == T_PARAM_END) // single parameter
        {
            if (G_optimizer.assigned_var) // known assigned variable
            {
                lreader_remove();
                lpartial_insert_create(instruction, (token_data_t){0,});
                lpartial_insert_create(T_IDF, (token_data_t){.var = G_optimizer.assigned_var});
                // use the value
                lpartial_insert_create(lreader_type(), lreader_data());
                lpartial_insert_create(T_ADR_ASSGN_END, (token_data_t){0,});
                lreader_remove();
            }
            else // result is returned on the stack
            {
                lreader_remove();
                lpartial_insert_create(instruction, (token_data_t){0,});
                lpartial_insert_create(T_IDF, (token_data_t){.var = get_tmp_var(0)});
                lpartial_insert_create(lreader_type(), lreader_data());
                // push the calculated value on the stack
                lpartial_insert_create(T_PUSHS, (token_data_t){0,});
                lpartial_insert_create(T_IDF, (token_data_t){.var = get_tmp_var(0)});
                lpartial_insert_create(T_ADR_ASSGN_END, (token_data_t){0,});
                lreader_remove();
            }
        }
        else
        {
            if (G_optimizer.assigned_var) // known assigned variable
            {
                htab_var_t tmp = G_optimizer.assigned_var;
                lpartial_insert_create(T_ADDRESS_ASSGN, (token_data_t){0,});
                G_optimizer.assigned_var = get_tmp_var(0);
                // convert the expression and assign the function parameter
                optimize_adress_assign_to_partial();
                lpartial_insert_create(T_ADR_ASSGN_END, (token_data_t){0,});
                lpartial_insert_create(instruction, (token_data_t){0,});
                lpartial_insert_create(T_IDF, (token_data_t){.var = tmp});
                lpartial_insert_create(T_REGISTER, (token_data_t){.string = "GF@?temp&1"});
                lpartial_insert_create(T_ADR_ASSGN_END, (token_data_t){0,});
                G_optimizer.assigned_var = NULL;
            }
            else // the result of the function call is pushed to the stack
            {
                lpartial_insert_create(T_ADDRESS_ASSGN, (token_data_t){0,});
                G_optimizer.assigned_var = get_tmp_var(0);
                // convert the expression and assign the function parameter
                optimize_adress_assign_to_partial();
                lpartial_insert_create(T_ADR_ASSGN_END, (token_data_t){0,});
                // execute the instruction
                lpartial_insert_create(instruction, (token_data_t){0,});
                lpartial_insert_create(T_REGISTER, (token_data_t){.string = "GF@?temp&1"});
                lpartial_insert_create(T_REGISTER, (token_data_t){.string = "GF@?temp&1"});
                // push the value on stack
                lpartial_insert_create(T_PUSHS, (token_data_t){0,});
                lpartial_insert_create(T_IDF, (token_data_t){.var = get_tmp_var(0)});
                lpartial_insert_create(T_ADR_ASSGN_END, (token_data_t){0,});
                G_optimizer.assigned_var = NULL;
            }
        }
    }
}

string_t get_program_header()
{
    // the predefined variables and registers
    return G_optimizer.predef_vars;
}

void optimize_input(uint8_t type)
{
    char frame[8] = " GF@";
    if (G_optimizer.recursive_function)
    {
        frame[1] = 'L'; // change the frame in case of resursive function
    }
    string_t code = string_const();
    static uint32_t counter = 1;
    char label1[32] = "";
    char label2[32] = "";
    // set new labels
    sprintf(label1, "in?success&%u", counter);
    sprintf(label2, "in?error&%u\n", counter++);

    lreader_reaload_saved_pos();
    lreader_remove_until(T_MULTI_ASSGN_END);
    htab_var_t assigned = vhtv_pop(G_optimizer.assigned_vars);

    // precompile the read instruction
    string_substr_push(&code, "READ", 4);
    if (!is_special_var(assigned))
    {
        string_substr_push(&code, frame, 4);
    }
    else
    {
        string_char_push(&code, ' ');
    }
    string_substr_push(&code, read_var_name(assigned), strlen(read_var_name(assigned)));
    string_char_push(&code, ' ');
    string_substr_push(&code, get_func_data_type(type), strlen(get_func_data_type(type)));
    string_char_push(&code, '\n');

    // if the second operand is not '_'
    if (!is_special_var(vhtv_get_last(G_optimizer.assigned_vars)))
    {
        // test the result of the READ instruction
        string_substr_push(&code, "TYPE GF@?temp&2 ", 16);
        string_substr_push(&code, frame, 4);
        string_substr_push(&code, read_var_name(assigned), strlen(read_var_name(assigned)));
        string_char_push(&code, '\n');
        string_substr_push(&code, "JUMPIFEQ ", 9);
        string_substr_push(&code, label1, strlen(label1));
        string_char_push(&code, ' ');
        string_substr_push(&code, "GF@?temp&2 string@", 18);
        string_substr_push(&code, get_func_data_type(type), strlen(get_func_data_type(type)));
        string_char_push(&code, '\n');
        string_substr_push(&code, "MOVE", 4);
        string_substr_push(&code, frame, 4);
        string_substr_push(&code, read_var_name(vhtv_get_last(G_optimizer.assigned_vars)), strlen(read_var_name(vhtv_get_last(G_optimizer.assigned_vars))));
        string_substr_push(&code, " int@1\n", 7);
        string_substr_push(&code, "JUMP ", 5);
        string_substr_push(&code, label2, strlen(label2));
        string_substr_push(&code, "LABEL ", 6);
        string_substr_push(&code, label1, strlen(label1));
        string_char_push(&code, '\n');
        string_substr_push(&code, "MOVE", 4);
        string_substr_push(&code, frame, 4);
        string_substr_push(&code, read_var_name(vhtv_get_last(G_optimizer.assigned_vars)), strlen(read_var_name(vhtv_get_last(G_optimizer.assigned_vars))));
        string_substr_push(&code, " int@0\n", 7);
        string_substr_push(&code, "LABEL ", 6);
        string_substr_push(&code, label2, strlen(label2));
    }
    // save the precompiled string for the generator
    lreader_n_replace(T_PRECOMPILED, (token_data_t){.string = string_read(code)});
}

void optimize_return()
{
    while(lreader_n_type() != T_RETURN_END)
    {
        if (lreader_type() == T_FUNC_CALL)
        {
            // optimize functional calls used in return statement
            optimize_func_call();
            if (lreader_n_type() == T_RETURN_END)
            {
                break;
            }
        }
        lreader_next();
    }
}
