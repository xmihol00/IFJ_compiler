
//=========================================================================================================
// File:        generator.c
// Case:        VUT, FIT, IFJ, compiler project
// Date:        9. 12. 2020
// Author(s):   David Mihola, Vladislav Sokolovskii
// Contac(s):   xmihol00@stud.fit.vutbr.cz, xsokol15@stud.fit.vutbr.cz
// Compiled:    gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)
// Description: File contanting definitions of functions responsible for taret IFJ20code generation.
//==========================================================================================================

#include "generator_private.h"

#define PRINST(...) do { fprintf(stdout, __VA_ARGS__); puts(""); } while (0) //adds new line character to the end of the giving
// string and prints it
#define PRCODE(...) do { fprintf(stdout, __VA_ARGS__); } while (0) //adds new line character to the end of the giving
//#define PRCODE(str) fprintf(stdout, str) //prints a piece of the instruction in target code

#define INFO_PRINT(INFO) puts(INFO)

struct {
    htab_func_t defined_func;       ///function that is currently being processed
    htab_func_t last_called_func;   ///last called function
    bool is_defined_func_recursive; ///hold the information if the defined function is recursive or not, used for the optimalization
    vect_htab_func_t func_calls;    ///stack that holds called functions, mainly used for the support of nested function calls
    int fun_depth;                  ///holds the depth of nested function calls

    uint8_t ttype;                  ///type of the token that is currently being processed
    token_data_t tdata;             ///data of the token that is currently being processed

    int32_t block_count;            ///the number of if-else blocks
    int32_t cond_branch_cnt;        ///counter of branches inside every separate else-if/else blocks
    int32_t condition_block_cnt;    ///counter of condition blocks
    vect_int32_t nested_conditions; ///stack of ordinal numbers of if-else conditions
    vect_int32_t nested_blocks;     ///stack of ordinal numbers of if-else blocks

    uint32_t cycle_cnt;             ///counter of the cycles in the given source file
    vect_int32_t nested_cycles;     ///stack of ordinal numbers of cycle blocks

    bool multi_assgn;               ///holds the information if T_MULTI_ASSIGN token was read
    bool adress_assgn;
    uint8_t adress_count;
} G_generator = {0,};

char *stack_inst[256] = {[NEG] "SUBS", [POS]"", [NOT] "NOTS", [POS_NOT] "", 
                         [CONCAT] "POPS GF@?temp&6\nPOPS GF@?temp&5\nCONCAT GF@?temp&5 GF@?temp&5 GF@?temp&6\nPUSHS GF@?temp&5", 
                         [IDIV] "IDIVS", 
                         [MUL] "MULS", "DIVS", [ADD] "ADDS", "SUBS", [LT] "LTS", "GTS\nNOTS", "GTS", 
                         "LTS\nNOTS", "EQS", "EQS\nNOTS", [AND] "ANDS", [OR] "ORS"};

char *address_inst[256] = {[NEG] "SUB ", [POS]"", [NOT] "NOT ", [POS_NOT] "", [CONCAT] "CONCAT ", [IDIV] "IDIV ", 
                         [MUL] "MUL ", "DIV ", [ADD] "ADD ", "SUB ", [LT] "LT ", "GT ", "GT ", 
                         "LT ", "EQ ", "EQ ", [AND] "AND ", [OR] "OR "};

void init_generator()
{
    G_generator.fun_depth = 0;
    G_generator.cycle_cnt = 0;
    G_generator.nested_cycles = vect_int32_const();

    G_generator.cond_branch_cnt = 0;
    G_generator.condition_block_cnt = 0;
    G_generator.nested_blocks = vect_int32_const();
    G_generator.nested_conditions = vect_int32_const();
    G_generator.func_calls = vect_htab_func_const();
}

void dispose_generator()
{
    discard_int32_vector(&G_generator.nested_cycles);
    discard_int32_vector(&G_generator.nested_blocks);
    discard_int32_vector(&G_generator.nested_conditions);
    discard_htab_func_vector(&G_generator.func_calls);
}

void generator()
{
    init_generator();

    generate_header();

    lreader_head(); // moves head before the first token from the list

    do {
        // read next token
        G_generator.tdata = lread_next(&G_generator.ttype);

        switch (G_generator.ttype) {
            case T_FUNC_DEF:
                generate_func_header();
                break;

            case T_IF: // start of if expression
                INFO_PRINT("\n# if statement start");
                generate_new_if_block();
                break;

            case T_IF_BODY: // end of if condition
                if (G_generator.adress_assgn) // jump was created by the optimizer using the adress jumping
                {
                    G_generator.adress_assgn = false;
                }
                else
                {
                    // jump from stack
                    printf("PUSHS bool@true\nJUMPIFNEQS $%s_if%d_%d\n", read_func_name(G_generator.defined_func),
                    vi32_get_last(G_generator.nested_blocks), G_generator.cond_branch_cnt);
                }
                break;
            
            case T_ELIF_BODY: // else if condition end
                if (G_generator.adress_assgn) // jump was created by the optimizer using the adress jumping
                {
                    G_generator.adress_assgn = false;
                }
                else
                {
                    // jump from stack
                    printf("PUSHS bool@true\nJUMPIFNEQS $%s_if%d_%d\n", read_func_name(G_generator.defined_func),
                    vi32_get_last(G_generator.nested_blocks), G_generator.cond_branch_cnt);
                }
                break;
            
            case T_FOR_BODY: // end of for condition (FOR3 was moved at the and of the for loop)
                if (G_generator.adress_assgn)
                {
                    G_generator.adress_assgn = false;
                }
                else
                {
                    printf("PUSHS bool@true\nJUMPIFNEQS $%s_forend%d\n", 
                    read_func_name(G_generator.defined_func), G_generator.cycle_cnt);
                    vi32_push(&G_generator.nested_cycles, G_generator.cycle_cnt);
                }
                break;

            case T_JMP_IF_EQ: // if/else if adress jump start
                G_generator.adress_count++;
                // operands will be added in following loops
                printf("JUMPIFEQ $%s_if%d_%d ", read_func_name(G_generator.defined_func),
                    vi32_get_last(G_generator.nested_blocks), G_generator.cond_branch_cnt);
                break;

            case T_JMP_IF_NEQ: // if/else if adress jump start
                G_generator.adress_count++;
                // operands will be added in following loops
                printf("JUMPIFNEQ $%s_if%d_%d ", read_func_name(G_generator.defined_func),
                    vi32_get_last(G_generator.nested_blocks), G_generator.cond_branch_cnt);
                break;
            
            case T_JMP_FOR_EQ: // for adress jump start
                G_generator.adress_count++;
                printf("JUMPIFEQ $%s_forend%d ", read_func_name(G_generator.defined_func), G_generator.cycle_cnt);
                vi32_push(&G_generator.nested_cycles, G_generator.cycle_cnt);
                break;
            
            case T_JMP_FOR_NEQ: // for adress jump start
                G_generator.adress_count++;
                printf("JUMPIFNEQ $%s_forend%d ", read_func_name(G_generator.defined_func), G_generator.cycle_cnt);
                vi32_push(&G_generator.nested_cycles, G_generator.cycle_cnt);
                break;

            case T_SINGLE_VAL_IF: // if condition is a single boolean value
                if (lreader_type() == T_BOOL)
                {
                    // the value is known
                    // read the next token
                    G_generator.tdata = lread_next(&G_generator.ttype);
                    if (!G_generator.tdata.boolean) // value is false
                    {
                        // unconditioned jump
                        printf("JUMP $%s_if%d_%d\n", read_func_name(G_generator.defined_func),
                        vi32_get_last(G_generator.nested_blocks), G_generator.cond_branch_cnt);
                    } // else do not jump
                    // reset adress expression counter
                    G_generator.adress_count = 0;
                }
                else // the value is unknown
                {
                    printf("JUMPIFNEQ $%s_if%d_%d bool@true ", read_func_name(G_generator.defined_func),
                    vi32_get_last(G_generator.nested_blocks), G_generator.cond_branch_cnt);
                    G_generator.adress_count = 2;
                }
                break;
            
            case T_SINGLE_VAL_FOR: // same as if above
            if (lreader_type() == T_BOOL)
            {
                G_generator.tdata = lread_next(&G_generator.ttype);
                if (!G_generator.tdata.boolean)
                {
                    printf("JUMP $%s_forend%d\n", read_func_name(G_generator.defined_func), G_generator.cycle_cnt);
                }
                G_generator.adress_count = 0;
                vi32_push(&G_generator.nested_cycles, G_generator.cycle_cnt);
            }
            else
            {
                printf("JUMPIFNEQ $%s_forend%d bool@true ", read_func_name(G_generator.defined_func), G_generator.cycle_cnt);
                vi32_push(&G_generator.nested_cycles, G_generator.cycle_cnt);
                G_generator.adress_count = 2;
            }
            break;

            case T_IF_BODY_END:
            case T_ELIF_BODY_END:
                // end of if/else if body
                generate_if_jumps();
                break;

            case T_ELSE_BODY_END:  // end of else body
                // get the correct branch count
                G_generator.cond_branch_cnt = vi32_pop(G_generator.nested_conditions);
                printf("LABEL $%s_ifend%d\n", read_func_name(G_generator.defined_func), vi32_get_last(G_generator.nested_blocks));
                // close the condition
                G_generator.condition_block_cnt = vi32_pop(G_generator.nested_blocks);
                INFO_PRINT("# if statement end\n");
                break;

            case T_FOR1:
                INFO_PRINT("\n# for statement start");
                G_generator.cycle_cnt++; // increase number of nested cycles
                break;
            
            case T_FOR2:
                // create the for start label
                PRINST("LABEL $%s_for%d", read_func_name(G_generator.defined_func), G_generator.cycle_cnt);
                break;

            case T_FOR_BODY_END:
                // end of for cycle
                generate_for_loop();
                INFO_PRINT("# for statement end\n");
                break;

            case T_RETURN_END:
                generate_return();
                break;
            
            case T_FUNC_BODY_END:
                // if the function is not void two returns will be generated,
                //which is not a problem as the second one will be always skipped
                generate_return(); 
                printf("# $%s: definition end\n\n", read_func_name(G_generator.defined_func));
                break;
            
            case T_VAR_DEF:
                // read the next token
                G_generator.tdata = lread_next(&G_generator.ttype);
                if (!is_special_var(G_generator.tdata.var))
                {
                    printf("DEFVAR %s%s\n", get_var_frame(G_generator.tdata.var) ? "GF@" : "LF@", 
                                            read_var_name(G_generator.tdata.var));
                }
                break;

            case T_FUNC_CALL:
                // store the called function
                vfht_push(&G_generator.func_calls, lread_next(&G_generator.ttype).func);
                break;
            
            case T_FCALL_END:
                // call the last stored function
                generate_function_call();
                break;
            
            case T_PARAM_END:
                // solving unoptimized print() function 
                try_generate_print();
                break;
            
            case T_ADDRESS_ASSGN:
                // set state to adress assign
                G_generator.adress_assgn = true;
                G_generator.adress_count = 0;
                if (lreader_type() == T_MOV) // if the next token is representing assignment
                {
                    G_generator.adress_count++;
                    lreader_next(); // skip the next token
                    printf("MOVE ");
                }
                break;
            
            case T_ADR_ASSGN_END:
                // end of adress assign
                G_generator.adress_assgn = false;
                break;

            case T_OP:
                if (G_generator.adress_assgn) // adress assign active
                {
                    // generate corrent instruction
                    printf("%s", address_inst[G_generator.tdata.op]);
                    if (G_generator.tdata.op == NOT) // not has only two addresses
                    {
                        G_generator.adress_count = 1; // skip one adress
                    }
                    else
                    {
                        G_generator.adress_count = 0;
                    }
                }
                else
                {
                    // stack instruction
                    printf("%s\n", stack_inst[G_generator.tdata.op]);
                }
                break;
            
            case T_REGISTER:
                if (++G_generator.adress_count < 3)
                {
                    // not the last adress
                    printf("%s ", G_generator.tdata.string);
                }
                else
                {
                    // the last adress must be followed by new line
                    printf("%s\n", G_generator.tdata.string);
                    G_generator.adress_count = 0;
                }
                break;
            
            // generation of operands
            case T_INT:
                generate_int();
                break;

            case T_FLOAT:
                generate_float();
                break;

            case T_STRING:
                generate_string();
                break;

            case T_BOOL:
                generate_bool();
                break;

            case T_IDF:
                generate_idf();
                break;
            // operand generation end
            
            case T_MULTI_ASSGN:
                G_generator.multi_assgn = true;
                if (lreader_type() == T_IDF)
                {
                    // skip auxiliary tokens needed for optimization
                    skip_token_block(T_VARS_END);
                }
                break;
            
            case T_MULTI_ASSGN_END:
                G_generator.multi_assgn = false;
                break;

            case T_ASSGN:
                if (lreader_n_type() == T_VARS_END || lreader_n_type() == T_VARS_DUMMY)
                {
                    // skip auxiliary tokens needed for optimization
                    lreader_next();
                    lreader_next();
                }
                break;

            case T_ASSGN_END:
                // read the next token 
                G_generator.tdata = lread_next(&G_generator.ttype);
                if (is_special_var(G_generator.tdata.var))
                {
                    // special variable
                    printf("POPS GF@?_&\n");
                }
                else
                {
                    // 
                    printf("POPS %s%s\n", get_var_frame(G_generator.tdata.var) ? "GF@" : "LF@",
                                            read_var_name(G_generator.tdata.var));
                }
                break;

            // direct IFJ20code instructions
            case T_WRITE_INST:
                G_generator.adress_assgn = true;
                G_generator.adress_count = 2;
                printf("WRITE ");
                break;
            
            case T_LEN_INST:
                G_generator.adress_assgn = true;
                G_generator.adress_count = 1;
                printf("STRLEN ");
                break;
            
            case T_I2F_INST:
                G_generator.adress_assgn = true;
                G_generator.adress_count = 1;
                printf("INT2FLOAT ");
                break;

            case T_F2I_INST:
                G_generator.adress_assgn = true;
                G_generator.adress_count = 1;
                printf("FLOAT2INT ");
                break;
            
            case T_I2FS_INST:
                printf("INT2FLOATS\n");
                break;
            
            case T_F2IS_INST:
                printf("FLOAT2INTS\n");
                break;
            
            case T_PUSHS:
                G_generator.adress_assgn = true;
                G_generator.adress_count = 2;
                printf("PUSHS ");
                break;
            // end of direct instructions
            
            case T_PRECOMPILED: // precompiled READ by the optimizer
                printf("# input\n%s\n", G_generator.tdata.string);
                break;

            default:
                break;
        }
        
    }while (llist_is_not_empty());
}

void try_generate_print()
{
    // if the function is print, generate the instructions
    if (is_build_in_function(vfht_get_last(G_generator.func_calls)) == T_PRINT)
    {
        printf("POPS GF@?temp&1\n");
		printf("WRITE GF@?temp&1\n");
    }
}

void generate_idf()
{
    if (G_generator.adress_assgn) // adress calculation
    {
        if (is_special_var(G_generator.tdata.var)) // special variable
        {
            if (++G_generator.adress_count < 3)
            {
                printf("GF@?_& ");
            }
            else
            {
                printf("GF@?_&\n");
                G_generator.adress_count = 0;
            }
        }
        else // regular variable
        {
            if (++G_generator.adress_count < 3)
            {
                printf("%s%s ", get_var_frame(G_generator.tdata.var) ? "GF@" : "LF@",
                read_var_name(G_generator.tdata.var));
            }
            else
            {
                printf("%s%s\n", get_var_frame(G_generator.tdata.var) ? "GF@" : "LF@",
                read_var_name(G_generator.tdata.var));
                G_generator.adress_count = 0;
            }
        }
    }
    else //stack calculation
    {
        if (is_special_var(G_generator.tdata.var))
        {
            printf("PUSHS GF@?_&\n");
        }
        else
        {
            printf("PUSHS %s%s\n", get_var_frame(G_generator.tdata.var) ? "GF@" : "LF@",
                read_var_name(G_generator.tdata.var));
        }
    }
}

void generate_bool()
{
    if (G_generator.adress_assgn) // adress calcualtion
    {
        if (++G_generator.adress_count < 3)
        {
            // not last operand
            printf("bool@%s ", G_generator.tdata.boolean ? "true" : "false");
        }
        else
        {   
            // last oprand
            printf("bool@%s\n", G_generator.tdata.boolean ? "true" : "false");
            G_generator.adress_count = 0;
        }
    }
    else // stack calculation
    {
        printf("PUSHS bool@%s\n", G_generator.tdata.boolean ? "true" : "false");
    }
}

void generate_int() // same as bool
{
    if (G_generator.adress_assgn)
    {
            if (++G_generator.adress_count < 3)
        {
            printf("int@%ld ", G_generator.tdata.integer);
        }
        else
        {
            printf("int@%ld\n", G_generator.tdata.integer);
            G_generator.adress_count = 0;
        }
    }
    else
    {
        printf("PUSHS int@%ld\n", G_generator.tdata.integer);
    }
}

void generate_string() // same as bool
{
    if (G_generator.adress_assgn)
    {
        if (++G_generator.adress_count < 3)
        {
            printf("string@%s ", G_generator.tdata.string);
        }
        else
        {
            printf("string@%s\n", G_generator.tdata.string);
            G_generator.adress_count = 0;
        }
    }
    else
    {
        printf("PUSHS string@%s\n", G_generator.tdata.string);
    }
}

void generate_float() //same as bool
{
    if (G_generator.adress_assgn)
    {
            if (++G_generator.adress_count < 3)
        {
            printf("float@%a ", G_generator.tdata.float64);
        }
        else
        {
            printf("float@%a\n", G_generator.tdata.float64);
            G_generator.adress_count = 0;
        }
    }
    else
    {
        printf("PUSHS float@%a\n", G_generator.tdata.float64);
    }
}

void generate_header()
{
    // header of the IFJ20code program, compiler variables
    printf(".IFJcode20\n");
    printf("\n# compiler variables definition\nDEFVAR GF@?temp&1\nDEFVAR GF@?temp&2\n");
    printf("DEFVAR GF@?temp&3\nDEFVAR GF@?temp&4\nDEFVAR GF@?temp&5\nDEFVAR GF@?temp&6\n");
    printf("DEFVAR GF@?_&\n\n");
    if (get_program_header() != NULL)
    {
        // variables defined by the optimizer
        printf("# variables of non recursive functions definition\n");
        printf("%s\n", string_read(get_program_header()));
        dispose_optimizer();
    }
    printf("JUMP $main\n");
}

void generate_func_header()
{
    G_generator.tdata = lread_next(&G_generator.ttype);//read next token with function name
    G_generator.defined_func = G_generator.tdata.func;
    // set flags based on the function type non-recursive/recursive
    G_generator.is_defined_func_recursive = func_is_recursive(G_generator.defined_func);

    if (G_generator.is_defined_func_recursive)
    {
        // create frame in case of recursive function
        printf("\n# $%s: definition start\nLABEL $%s\nCREATEFRAME\nPUSHFRAME\n",
        read_func_name(G_generator.defined_func), read_func_name(G_generator.defined_func));
        // load function parameters
        vect_htab_var_t params = get_params(G_generator.defined_func);
        if (params != NULL)
        {
            // define the parameters and assign them with passed values
            for (int64_t i = vhtv_pos(params); i >= 0; i--) // from left to right
            {
                printf("DEFVAR LF@%s\n", read_var_name(vhtv_get_pos(params, i)));
                printf("POPS LF@%s\n", read_var_name(vhtv_get_pos(params, i)));
            }
        }
        vect_htab_var_t ret_vals = get_ret_vals(G_generator.defined_func);
        if (ret_vals != NULL)
        {
            // define name return values and assign them default 0 value
            for (int64_t i = 0; i <= vhtv_pos(ret_vals); i++)
            {
                printf("DEFVAR LF@%s\n", read_var_name(vhtv_get_pos(ret_vals, i)));
                printf("MOVE LF@%s %s\n", read_var_name(vhtv_get_pos(ret_vals, i)), get_var_def_value(vhtv_get_pos(ret_vals, i)));
            }
        }
    }
    else // not recursive function
    {
        printf("\n# $%s: definition start\nLABEL $%s\n",
        read_func_name(G_generator.defined_func), read_func_name(G_generator.defined_func));
        // parameters are already defined as global variables and are assigned by the callee

        // define name return values and assign them default 0 value
        vect_htab_var_t ret_vals = get_ret_vals(G_generator.defined_func);
        if (ret_vals != NULL)
        {
            for (int64_t i = 0; i <= vhtv_pos(ret_vals); i++)
            {
                printf("MOVE GF@%s %s\n", read_var_name(vhtv_get_pos(ret_vals, i)), get_var_def_value(vhtv_get_pos(ret_vals, i)));
            }
        }
    }

}

void generate_if_jumps()
{
    // load next token, but not move the reader
    G_generator.tdata = lcheck_next(&G_generator.ttype);
    if ((G_generator.ttype != T_ELIF && G_generator.ttype != T_ELSE)) 
    {
        // condition body end
        printf("LABEL $%s_if%d_%d\n", read_func_name(G_generator.defined_func),
        vi32_get_last(G_generator.nested_blocks), G_generator.cond_branch_cnt);
        printf("LABEL $%s_ifend%d\n", read_func_name(G_generator.defined_func), vi32_get_last(G_generator.nested_blocks));
        // reset the counters
        G_generator.condition_block_cnt = vi32_pop(G_generator.nested_blocks);
        G_generator.cond_branch_cnt = vi32_pop(G_generator.nested_conditions);
        INFO_PRINT("# if statement end\n");
        return;
    }
    // condition continues with else if/else
    printf("JUMP $%s_ifend%d\n", read_func_name(G_generator.defined_func), vi32_get_last(G_generator.nested_blocks));
    printf("LABEL $%s_if%d_%d\n", read_func_name(G_generator.defined_func),
    vi32_get_last(G_generator.nested_blocks), G_generator.cond_branch_cnt);
    // increase the numer of branches
    G_generator.cond_branch_cnt++;
}

void generate_new_if_block() 
{
    // increase the number of nested condition blocks
    G_generator.block_count++;
    // get the right bloc count
    G_generator.condition_block_cnt = G_generator.block_count;
    // push current values on the stack
    vi32_push(&G_generator.nested_conditions, G_generator.cond_branch_cnt);
    vi32_push(&G_generator.nested_blocks, G_generator.condition_block_cnt);
    // reset the branch count
    G_generator.cond_branch_cnt = 1;
}

void generate_return()
{
    vect_htab_var_t ret_vals = get_ret_vals(G_generator.defined_func);

    if (G_generator.is_defined_func_recursive) // recursive function
    {
        if (ret_vals != NULL)
        {
            for (int64_t i = 0; i <= vhtv_pos(ret_vals); i++)
            {
                // push return values
                printf("PUSHS LF@%s\n", read_var_name(vhtv_get_pos(ret_vals, i)));
            }
        }
        // frame has to be poped, in case main function replace return with exit
        printf("POPFRAME\n%s\n", strcmp(get_func_name(G_generator.defined_func), "main") ? "RETURN" : "EXIT int@0");
    }
    else // non recursive function
    {
        if (ret_vals != NULL)
        {
            for (int64_t i = 0; i <= vhtv_pos(ret_vals); i++)
            {
                // push return values
                printf("PUSHS GF@%s\n", read_var_name(vhtv_get_pos(ret_vals, i)));
            }
        }
        // frame was not created
        printf("%s\n", strcmp(get_func_name(G_generator.defined_func), "main") ? "RETURN" : "EXIT int@0");
    }
}

void generate_for_loop()
{
    // get the last cycle index
    int32_t cycle_index = vi32_pop(G_generator.nested_cycles);
    // generate the jump instructions
    printf("JUMP $%s_for%d\n", read_func_name(G_generator.defined_func), cycle_index);
    printf("LABEL $%s_forend%d\n", read_func_name(G_generator.defined_func), cycle_index);
}

void skip_token_block(uint8_t stop_token)
{
    // skip auxiliary tokens with no meaning for the generator
    while (lread_next(&G_generator.ttype), G_generator.ttype != stop_token);
}

void generate_function_call()
{
    if (vfht_pos(G_generator.func_calls) < 0)
    {
        return;
    }
    switch(is_build_in_function(vfht_get_last(G_generator.func_calls))) 
    {
        // built-in functions are inlined
        case T_INS: // inputs
            c_input(T_STRING);
            break;

        case T_INI: // inputi
            c_input(T_INT);
            break;

        case T_INB: // inputb
            c_input(T_BOOL);
            break;

        case T_INF: // inputf
            c_input(T_FLOAT);
            break;

        case T_SUBS: // substr
            c_substr();
            break;

        case T_ORD: // ord
            c_ord();
            break;

        case T_I2F: // int2float
            c_int2float();
            break;

        case T_F2I: // float2int
            c_float2int();
            break;

        case T_CHR: // chr
            c_chr();
            break;

        case T_LEN: // len
            c_len();
            break;
            
        case T_PRINT:
            break;

        default: // not an inbuilt funtion, therfore must be called
            printf("CALL $%s\n", get_func_name(vfht_get_last(G_generator.func_calls)));
            break;
    }
    vfht_pop(G_generator.func_calls); // pop the called function from the call stack
}
