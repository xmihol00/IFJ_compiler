
//=========================================================================================================
// File:        parser.c
// Case:        VUT, FIT, IFJ, compiler project
// Date:        9. 12. 2020
// Author(s):   David Mihola
// Contac(s):   xmihol00@stud.fit.vutbr.cz
// Compiled:    gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)
// Description: File containing defintions of functions responsible for the IFJ20 sourcde code
//              parsing and syntax analysis.
//==========================================================================================================

#include "parser_private.h"

/**
 * @struct the representation of the syntax control unit
**/
struct parser
{
    uint8_t state;                 /// the current state of the parses
    uint8_t expression;            /// ongoing expression
    uint8_t curly_bracket;         /// current type of open curly brcket
    uint8_t eq_type;               /// type of EQ in an assign expression
    uint8_t last_type;             /// type of the last added token
    bool undef_var;                /// holds an information if at least one variable is undefined in a multi var assign expression
    uint32_t ongoing_for_count;    /// the number of currently ongoing for cycles
    int32_t scope_level;           /// the current scope level
    vect_uint8_t expr_stack;       /// the stack which holds the ongoing expressions
    vect_uint8_t cb_stack;         /// the stack which holds the currently open curly bracket
    vect_htab_var_t for_variables; /// for_variables declared in the current ongoing for cycles
    htab_func_t function;               /// the function which is currently defined
    vect_htab_func_t called_funcs; /// stack of function calls
    vect_htab_var_t assigned_var;  /// vector of currently assigned variables
} * G_parser;

void syntax_analysis()
{
    // the syntax LL(1) table
    void_func_t syntax_table[33][16] = {
    /**  OP       IDF        FUNCTION   STRING   KEYWORD   TYPE        EQ       OCB {      CCB }     OB (      CB )      COMMA      NEW LINE   SEMI      CONSTANT **/
        {syn_err, syn_err,   syn_err,   syn_err, in_pckg,  syn_err,    syn_err, syn_err,   syn_err,  syn_err,  syn_err,  syn_err,   in_nl,    syn_err,  syn_err}, 
        {syn_err, pckg_midf, syn_err,   syn_err, syn_err,  syn_err,    syn_err, syn_err,   syn_err,  syn_err,  syn_err,  syn_err,   pckg_nl,  syn_err,  syn_err},
        {syn_err, syn_err,   syn_err,   syn_err, syn_err,  syn_err,    syn_err, syn_err,   syn_err,  syn_err,  syn_err,  syn_err,   nlg_nl,   syn_err,  syn_err},
        {syn_err, syn_err,   syn_err,   syn_err, fnn_func, syn_err,    syn_err, syn_err,   syn_err,  syn_err,  syn_err,  syn_err,   fnn_nl,   syn_err,  syn_err},
        {syn_err, syn_err,   fnnm_fidf, syn_err, syn_err,  syn_err,    syn_err, syn_err,   syn_err,  syn_err,  syn_err,  syn_err,   syn_err,  syn_err,  syn_err},
        {syn_err, fpf_idf,   syn_err,   syn_err, syn_err,  syn_err,    syn_err, syn_err,   syn_err,  syn_err,  fpf_cb,   syn_err,   fpf_nl,   syn_err,  syn_err},
        {syn_err, fprm_idf,  syn_err,   syn_err, syn_err,  syn_err,    syn_err, syn_err,   syn_err,  syn_err,  syn_err,  syn_err,   fprm_nl,  syn_err,  syn_err},
        {syn_err, syn_err,   syn_err,   syn_err, syn_err,  fpt_type,   syn_err, syn_err,   syn_err,  syn_err,  syn_err,  syn_err,   syn_err,  syn_err,  syn_err},
        {syn_err, syn_err,   syn_err,   syn_err, syn_err,  syn_err,    syn_err, syn_err,   syn_err,  syn_err,  fnc_cb,   fnc_cmm,   syn_err,  syn_err,  syn_err},
        {syn_err, syn_err,   syn_err,   syn_err, syn_err,  fncrt_type, syn_err, fncrt_ocb, syn_err,  fncrt_ob, syn_err,  syn_err,   syn_err,  syn_err,  syn_err},
        {syn_err, frtob_idf, syn_err,   syn_err, syn_err,  frtob_type, syn_err, syn_err,   syn_err,  syn_err,  frtob_cb, syn_err,   frtob_nl, syn_err,  syn_err},
        {syn_err, syn_err,   syn_err,   syn_err, syn_err,  syn_err,    syn_err, syn_err,   syn_err,  syn_err,  rtc_cb,   rtc_comm,  syn_err,  syn_err,  syn_err},
        {syn_err, syn_err,   syn_err,   syn_err, syn_err,  mrt_type,   syn_err, syn_err,   syn_err,  syn_err,  syn_err,  syn_err,   mrt_nl,   syn_err,  syn_err},
        {syn_err, syn_err,   syn_err,   syn_err, syn_err,  mnrt_type,  syn_err, syn_err,   syn_err,  syn_err,  syn_err,  syn_err,   syn_err,  syn_err,  syn_err},
        {syn_err, syn_err,   syn_err,   syn_err, syn_err,  syn_err,    syn_err, syn_err,   syn_err,  syn_err,  mnrc_cb,  mnrc_comm, syn_err,  syn_err,  syn_err},
        {syn_err, rtn_idf,   syn_err,   syn_err, syn_err,  syn_err,    syn_err, syn_err,   syn_err,  syn_err,  syn_err,  syn_err,   rtn_nl,   syn_err,  syn_err},
        {syn_err, syn_err,   syn_err,   syn_err, syn_err,  syn_err,    syn_err, ocbn_ocb,  syn_err,  syn_err,  syn_err,  syn_err,   syn_err,  syn_err,  syn_err},
        {syn_err, expr_idf,  expr_fidf, syn_err, expr_kw,  syn_err,    syn_err, syn_err,   expr_ccb, syn_err,  syn_err,  syn_err,   expr_nl,  syn_err,  syn_err},
        {syn_err, syn_err,   syn_err,   syn_err, ees_kw,   syn_err,    syn_err, syn_err,   syn_err,  syn_err,  syn_err,  syn_err,   ees_nl,   syn_err,  syn_err},
        {syn_err, syn_err,   syn_err,   syn_err, eel_kw,   syn_err,    syn_err, eel_ocb,   syn_err,  syn_err,  syn_err,  syn_err,   syn_err,  syn_err,  syn_err},
        {syn_err, syn_err,   syn_err,   syn_err, syn_err,  syn_err,    syn_err, syn_err,   syn_err,  syn_err,  syn_err,  syn_err,   nln_nl,   syn_err,  syn_err},
        {syn_err, syn_err,   syn_err,   syn_err, syn_err,  syn_err,    fv_eq,   syn_err,   syn_err,  syn_err,  syn_err,  fv_cmm,    syn_err,  syn_err,  syn_err},
        {syn_err, mfv_idf,   syn_err,   syn_err, syn_err,  syn_err,    syn_err, syn_err,   syn_err,  syn_err,  syn_err,  syn_err,   mfv_nl,   syn_err,  syn_err},
        {syn_err, syn_err,   syn_err,   syn_err, syn_err,  syn_err,    mfvc_eq, syn_err,   syn_err,  syn_err,  syn_err,  mfvc_cmm,  syn_err,  syn_err,  syn_err},
        {psa_op,  psa_idf,   psa_fidf,  psa_str, syn_err,  syn_err,    syn_err, psa_ocb,   syn_err,  psa_ob,   psa_cb,   psa_cmm,   psa_nl,   psa_semi, psa_num},
        {syn_err, f1_id,     syn_err,   syn_err, syn_err,  syn_err,    syn_err, syn_err,   syn_err,  syn_err,  syn_err,  syn_err,   f1_nl,    f1_semi,  syn_err},
        {syn_err, syn_err,   syn_err,   syn_err, syn_err,  syn_err,    f1a_eq,  syn_err,   syn_err,  syn_err,  syn_err,  f1a_comm,  syn_err,  syn_err,  syn_err},
        {syn_err, f1ma_id,   syn_err,   syn_err, syn_err,  syn_err,    syn_err, syn_err,   syn_err,  syn_err,  syn_err,  syn_err,   f1ma_nl,  syn_err,  syn_err},
        {syn_err, syn_err,   syn_err,   syn_err, syn_err,  syn_err,    f1c_eq,  syn_err,   syn_err,  syn_err,  syn_err,  f1c_comm,  syn_err,  syn_err,  syn_err},
        {syn_err, f3_id,     syn_err,   syn_err, syn_err,  syn_err,    syn_err, f3_ocb,    syn_err,  syn_err,  syn_err,  syn_err,   f3_nl,    syn_err,  syn_err},
        {syn_err, syn_err,   syn_err,   syn_err, syn_err,  syn_err,    f3a_eq,  syn_err,   syn_err,  syn_err,  syn_err,  f3a_comm,  syn_err,  syn_err,  syn_err},
        {syn_err, f3ma_id,   syn_err,   syn_err, syn_err,  syn_err,    syn_err, syn_err,   syn_err,  syn_err,  syn_err,  syn_err,   f3ma_nl,  syn_err,  syn_err},
        {syn_err, syn_err,   syn_err,   syn_err, syn_err,  syn_err,    f3c_eq,  syn_err,   syn_err,  syn_err,  syn_err,  f3c_comm,  syn_err,  syn_err,  syn_err},
        };

    string_t token_data = string_const();
    int token_type;

    while ((token_type = scanner(&token_data)) != EOF)
    {
        // sellect the correct function
        syntax_table[G_parser->state][token_type](token_data);

        // remembering the last non new line token type to correctly parse new lines in expression
        G_parser->last_type = token_type;

    }

    if (G_parser->state != FUNCTION_NEXT && G_parser->state != NL_NEXT_GLOBAL)
    {
        // to correctly end the syntax analysis the parser should be in state FUNCTION_NEXT,
        // but we also allow the parsing to end correctly, when EOF is directly behind the function body
        exit(SYNTAX_ERR); // all other states evokes error
    }

    // check for undefined functions
    all_func_have_definition();   

    // parser and the expression analyser is not needed anymore
    dispose_parser();
    dispose_orpn();
}

void init_parser()
{
    G_parser = calloc(1, sizeof(struct parser));
    if (G_parser == NULL)
        exit(MEM_ERR);

    G_parser->expr_stack = vect_uint8_const();
    vu8_push(&G_parser->expr_stack, EMPTY);
    G_parser->cb_stack = vect_uint8_const();
    G_parser->for_variables = vect_htab_var_const();
    G_parser->called_funcs = vect_htab_func_const();
    G_parser->assigned_var = vect_htab_var_const();
}

void dispose_parser()
{
    if (G_parser != NULL)
    {
        discard_uint8_vector(&G_parser->expr_stack);
        discard_uint8_vector(&G_parser->cb_stack);
        discard_htab_var_vector(&G_parser->for_variables);
        discard_htab_var_vector(&G_parser->assigned_var);
        discard_htab_func_vector(&G_parser->called_funcs);
        free(G_parser);
        G_parser = NULL;
    }
}

// **************** THE PARSER FUNCTIONS USED IN THE SYNTAX MATRIX *****************

// =================================== ERROR =======================================

void syn_err(string_t data)
{
    (void)data;
    exit(SYNTAX_ERR);
}

// ================================== INITIAL ======================================

void in_nl(string_t data)
{
    (void)data;
    return;
}

void in_pckg(string_t data)
{
    if (string_char_pop(data) == PACKAGE)
    {
        G_parser->state = MAIN_PCKG_NEXT;
    }
    else
    {
        exit(SYNTAX_ERR); //no package declaration
    }
}

// ============================ MAIN PACKAGE NEXT ==================================

void pckg_midf(string_t data)
{
    if (strcmp(string_read(data), "main"))
    {
        exit(SYNTAX_ERR); // wrong package name
    }
    else
    {
        string_reset(data);
        G_parser->state = NL_NEXT_GLOBAL;
    }
}

void pckg_nl(string_t data)
{
    (void)data;
}

// ========================== NEW LINE NEXT GLOBAL =================================

void nlg_nl(string_t data)
{
    (void)data;
    G_parser->state = FUNCTION_NEXT;
}

// ============================= FUNCTION NEXT =====================================

void fnn_func(string_t data)
{
    if (string_char_pop(data) == FUNC)
    {
        G_parser->state = FUNCTION_NAME;
    }
    else
    {
        exit(SYNTAX_ERR); //Not a function key word
    }
}

void fnn_nl(string_t data)
{
    (void)data;
}

// ============================= FUNCTION NAME =====================================

void fnnm_fidf(string_t data)
{
    G_parser->function = find_func_ht(string_read(data));
    if (G_parser->function == NULL)
    {
        G_parser->function = insert_func_ht(string_read(data));
        attache_partial_htab(G_parser->function);
    }
    else if (get_func_state(G_parser->function) == F_UNDEF)
    {
        attache_partial_htab(G_parser->function);
    }
    else
    {
        exit(SEMANTIC_DEF_ERR); // redefinition of a function
    }

    set_func_state(G_parser->function, F_DEF);
    start_scope(++G_parser->scope_level);
    string_reset(data);
    G_parser->state = FUNC_PARAM_FIRST;
}

// ====================== FUNCTION FIRST PARAMETER NAME ============================

void fpf_idf(string_t data)
{
    htab_var_t var = insert_var_ht(string_read(data), 1);
    set_var_state(var, V_DEF);
    add_func_parameter(G_parser->function, var);
    string_reset(data);
    G_parser->state = FUNC_PARAM_TYPE;
}

void fpf_cb(string_t data)
{
    (void)data;
    G_parser->state = FUNC_RET_TYPE;
}

void fpf_nl(string_t data)
{
    (void)data;
}

// ========================= FUNCTION PARAMETER NAME ===============================

void fprm_idf(string_t data)
{
    htab_var_t var = insert_var_ht(string_read(data), 1);
    set_var_state(var, V_DEF);
    add_func_parameter(G_parser->function, var);
    string_reset(data);
    G_parser->state = FUNC_PARAM_TYPE;
}

void fprm_nl(string_t data)
{
    (void)data;
}

// ========================= FUNCTION PARAMETER TYPE ===============================

void fpt_type(string_t data)
{
    add_func_param_type(G_parser->function, string_char_pop(data));
    G_parser->state = FUNC_COMMA_END;
}

// ========================== FUNCTION COMMA OR END ================================

void fnc_cmm(string_t data)
{
    (void)data;
    G_parser->state = FUNC_PARAM_NAME;
}

void fnc_cb(string_t data)
{
    (void)data;
    G_parser->state = FUNC_RET_TYPE;
}

// ========================== FUNCTION RETURN TYPES ================================

void fncrt_type(string_t data)
{
    add_func_ret_val_type(G_parser->function, string_char_pop(data), false);
    G_parser->state = OCB_NEXT;
}

void fncrt_ob(string_t data)
{
    (void)data;
    G_parser->state = RET_TYPE_OB;
}

void fncrt_ocb(string_t data)
{
    return_check_function(FUNCTION_WITHOUT_RETURN);
    (void)data;
    vu8_push(&G_parser->cb_stack, NO_CB);
    G_parser->curly_bracket = FUNCTION_CB;
    G_parser->state = NL_NEXT;
    ltype_insert(T_FUNC_DEF);
    lfunc_insert(G_parser->function);
}

// ========================== FUNCTION RETURN TYPE OB ==============================

void frtob_type(string_t data)
{
    (void)data;
    add_func_ret_val_type(G_parser->function, string_char_pop(data), false);
    G_parser->state = RET_TYPE_COMMA;
}

void frtob_idf(string_t data)
{
    htab_var_t var = insert_var_ht(string_read(data), 0);
    set_var_state(var, V_DEF);
    add_func_ret_val(G_parser->function, var);
    string_reset(data);
    G_parser->state = NAMED_RET_TYPES;
}

void frtob_cb(string_t data)
{
    (void)data;
    G_parser->state = OCB_NEXT;
}

void frtob_nl(string_t data)
{
    (void)data;
}

// ============================= RETURN TYPE COMMA =================================

void rtc_comm(string_t data)
{
    (void)data;
    G_parser->state = MUL_RET_TYPES;
}

void rtc_cb(string_t data)
{
    (void)data;
    G_parser->state = OCB_NEXT;
}

// =========================== MULTIPLE RETURN TYPES ===============================

void mrt_type(string_t data)
{
    add_func_ret_val_type(G_parser->function, string_char_pop(data), false);
    G_parser->state = RET_TYPE_COMMA;
}

void mrt_nl(string_t data)
{
    (void)data;
}

// ======================== MULTIPLE NAMED RETURN TYPES ============================

void mnrt_type(string_t data)
{
    add_func_ret_val_type(G_parser->function, string_char_pop(data), true);
    G_parser->state = NRET_TYPE_COMMA;
}

// ========================= NAMED RETURN TYPES COMMA ==============================
void mnrc_comm(string_t data)
{
    (void)data;
    G_parser->state = RET_NAME;
}

void mnrc_cb(string_t data)
{
    (void)data;
    G_parser->state = OCB_NEXT;
}

// ================================ RETURN NAME ====================================

void rtn_idf(string_t data)
{
    htab_var_t var = insert_var_ht(string_read(data), 0);
    set_var_state(var, V_DEF);
    add_func_ret_val(G_parser->function, var);
    string_reset(data);
    G_parser->state = NAMED_RET_TYPES;
}

void rtn_nl(string_t data)
{
    (void)data;
}

// ========================= OPEN CURTLY BRACKET NEXT ==============================

void ocbn_ocb(string_t data)
{
    (void)data;
    vu8_push(&G_parser->cb_stack, NO_CB);
    G_parser->curly_bracket = FUNCTION_CB;
    G_parser->state = NL_NEXT;
    ltype_insert(T_FUNC_DEF);
    lfunc_insert(G_parser->function);
    if (get_ret_val_types(G_parser->function) == NULL)
    {
        return_check_function(FUNCTION_WITHOUT_RETURN);
    }
    else
    {
        return_check_function(FUNCTION_WITH_RETURN);
    }
}

// ================================= STATEMENT =====================================

void expr_kw(string_t data)
{
    switch (string_char_pop(data))
    {
    case IF:
        assign_constants_before_scope();
        copy_vars_to_new_scope();
        ltype_insert(T_IF);
        G_parser->expression = E_IF;
        G_parser->state = PSA;
        return_check_function(IF_START);
        return;

    case FOR:
        assign_constants_before_scope();
        copy_vars_to_new_scope();
        ltype_insert(T_FOR1);
        G_parser->expression = E_FOR1;
        G_parser->state = FOR1;
        start_scope(++G_parser->scope_level);
        if (G_parser->ongoing_for_count == 0)
        {
            save_first_for();
        }
        ltype_insert(T_ASSGN);
        G_parser->ongoing_for_count++;
        return;

    case RETURN:
        return_check_function(RETURNERINO);
        ltype_insert(T_RETURN);
        G_parser->expression = E_RETURN;
        G_parser->state = PSA;
        return;

    default:
        exit(SYNTAX_ERR); // other key words cannot be used in this context
    }
}

void expr_idf(string_t data)
{
    htab_var_t variable = find_var_ht(string_read(data));
    if (variable == NULL)
    {
        G_parser->undef_var = true;
        variable = insert_var_ht(string_read(data), G_parser->scope_level);

        if (G_parser->ongoing_for_count)
        {
            vhtv_push(&G_parser->for_variables, variable);
        }
        else
        {
            ltype_insert(T_VAR_DEF);
            lvar_insert(variable);
        }
    }

    ltype_insert(T_ASSGN);
    G_parser->state = ASSIGN;
    G_parser->expression = E_ASSGN;
    vhtv_push(&G_parser->assigned_var, variable);

    if (G_parser->ongoing_for_count)
    {
        unset_var_value(variable);
    }

    string_reset(data);
}

void expr_fidf(string_t data)
{
    vu8_push(&G_parser->expr_stack, G_parser->expression);
    vu8_push(&G_parser->expr_stack, E_FIRST_FUNC);
    G_parser->expression = E_PARAM;

    if (find_var_ht(string_read(data)))
    {
        exit(SYNTAX_ERR); // function call attem on a varianle, operand followed by close bracket without operator
    }

    htab_func_t function = find_func_ht(string_read(data));
    if (function == NULL)
    {
        function = insert_func_ht(string_read(data));
        set_func_state(function, F_UNDEF);
    }

    ltype_insert(T_FISRT_FCALL);
    add_func_call(G_parser->function, function);
    orpn_add_func(function);
    G_parser->state = PSA;
    string_reset(data);
}

void expr_ccb(string_t data)
{
    (void)data;
    switch (G_parser->curly_bracket)
    {
    case IF_CB:
        G_parser->state = STATEMENT_ELSE;
        assign_constants_after_scope();
        ltype_insert(T_IF_BODY_END);
        return_check_function(IF_END);
        break;

    case ELIF_CB:
        G_parser->state = STATEMENT_ELSE;
        assign_constants_after_scope();
        ltype_insert(T_ELIF_BODY_END);
        return_check_function(IF_END);
        break;

    case ELSE_CB:
        G_parser->state = NL_NEXT;
        assign_constants_after_scope();
        ltype_insert(T_ELSE_BODY_END);
        return_check_function(ELSE_END);
        remove_vars_values_in_scope();
        break;

    case FOR_CB:
        G_parser->state = NL_NEXT;
        end_scope(G_parser->scope_level--);
        lfor_body_end(T_FOR_BODY_END);
        if (--G_parser->ongoing_for_count == 0)
        {
            store_for_variables(G_parser->for_variables);
        }
        remove_vars_values_in_scope();
        break;

    case FUNCTION_CB:
        G_parser->state = NL_NEXT_GLOBAL;
        ltype_insert(T_FUNC_BODY_END);
        if (return_check_function(FUNCTION_BODY_END))
        {
            exit(SEMANTIC_PARAM_COUNT_ERR); // function does not return correctly
        }
        break;

    default:
        exit(SYNTAX_ERR); // more close curly brackets were used than the open ones
    }

    end_scope(G_parser->scope_level--);
    G_parser->curly_bracket = vu8_pop(G_parser->cb_stack);
}

void expr_nl(string_t data)
{
    (void)data;
}

// ============================== STATEMENT ELSE ===================================

void ees_kw(string_t data)
{
    if (string_char_pop(data) == ELSE)
    {
        copy_vars_reopened_scope();
        G_parser->expression = E_ELSE;
        G_parser->state = STATEMENT_ELIF;
        return;
    }

    exit(SYNTAX_ERR); // different key word than else cannot be used in this context
}

void ees_nl(string_t data)
{
    remove_vars_values_in_scope();
    (void)data;
    G_parser->state = STATEMENT;
}

// ============================== STATEMENT ELIF ===================================

void eel_kw(string_t data)
{
    if (string_char_pop(data) == IF)
    {
        return_check_function(ELIF_START);
        ltype_insert(T_ELIF);
        G_parser->expression = E_ELIF;
        G_parser->state = PSA;
    }
    else
        exit(SYNTAX_ERR); // different key word than if cannot be used in this context
}

void eel_ocb(string_t data)
{
    start_scope(++G_parser->scope_level);
    return_check_function(ELSE_START);
    (void)data;
    vu8_push(&G_parser->cb_stack, G_parser->curly_bracket);
    ltype_insert(T_ELSE);
    G_parser->curly_bracket = ELSE_CB;
    G_parser->state = NL_NEXT;
}

// ============================== NEW LINE NEXT ====================================

void nln_nl(string_t data)
{
    (void)data;
    G_parser->state = STATEMENT;
}

// ============================== FIRST VARIABLE ===================================

void fv_eq(string_t data)
{
    G_parser->eq_type = string_char_pop(data);
    
    if (G_parser->eq_type != EQ_DEF)
    {
        if (G_parser->undef_var)
        {
            exit(SEMANTIC_DEF_ERR); // canot assign to not defined variable
        }
    }
    else
    {
        if (!G_parser->undef_var)
        {
            htab_var_t var = vhtv_pop(G_parser->assigned_var);
            // variable can be redefined if the current scope is higher than the scope of an already defined variable
            if (get_var_scope(var) < G_parser->scope_level)
            {
                char *name = read_var_name(var);
                string_t str = string_const();
                // extract the variable named assigned by the programmer
                for (int64_t i = 0; name[i] != '&'; i++) // not very clean, but works :)
                {
                    string_char_push(&str, name[i]);
                }
                var = insert_var_ht(string_read(str), G_parser->scope_level);
                discard_string(&str);

                if (G_parser->ongoing_for_count)
                {
                    vhtv_push(&G_parser->for_variables, var);
                }
                else
                {
                    lchange_last(T_VAR_DEF);
                    lvar_insert(var);
                    ltype_insert(T_ASSGN);
                }
                vhtv_push(&G_parser->assigned_var, var);
            }
            else
            {
                exit(SEMANTIC_DEF_ERR); //redefinition of a variable
            }
        }
    }
                 
    lvar_insert(vhtv_get_last(G_parser->assigned_var));
    ltype_insert(T_VARS_END);
    G_parser->state = PSA;
    G_parser->undef_var = false;
}

void fv_cmm(string_t data)
{
    (void)data;
    G_parser->expression = E_MULTI_ASSGN;
    lchange_last(T_MULTI_ASSGN);
    G_parser->state = MULTI_ASSIGN;
}

// ========================== MULTIPLE FIRST VARIABLE ==============================

void mfv_idf(string_t data)
{
    htab_var_t variable = find_var_ht(string_read(data));
    if (variable == NULL)
    {
        G_parser->undef_var = true;
        variable = insert_var_ht(string_read(data), G_parser->scope_level);
        if (G_parser->ongoing_for_count)
        {
            vhtv_push(&G_parser->for_variables, variable);
        }
        else
        {
            lchange_last(T_VAR_DEF);
            lvar_insert(variable);
            ltype_insert(T_MULTI_ASSGN);
        }
    }
    else if (get_var_state(variable) == V_UNDEF && !is_special_var(variable))
    {
        exit(SEMANTIC_DEF_ERR); // repetetive definition of a variable in multi assign
    }

    vhtv_push(&G_parser->assigned_var, variable);
    G_parser->state = MUTLI_ASSIGN_COMMA;

    if (G_parser->ongoing_for_count)
    {
        unset_var_value(variable);
    }

    string_reset(data);
}

void mfv_nl(string_t data)
{
    (void)data;
}

// ======================= MULTIPLE FIRST VARIABLE COMMA ===========================

void mfvc_cmm(string_t data)
{
    (void)data;
    G_parser->state = MULTI_ASSIGN;
}

void mfvc_eq(string_t data)
{
    G_parser->eq_type = string_char_pop(data);
    if (G_parser->eq_type == EQ_ASSGN)
    {
        if (G_parser->undef_var)
        {
            exit(SEMANTIC_DEF_ERR); // canot use = on not defined variable
        }
    }
    else if (G_parser->eq_type == EQ_DEF)
    {
        if (!G_parser->undef_var)
        {
            exit(SEMANTIC_DEF_ERR); // canot use := on all defined variables
        }
    }
    else
    {
        exit(SYNTAX_ERR); // =+, =-, =*, =/ used on multi variable assignment
    }

    vhtv_load_to_list(G_parser->assigned_var);
    G_parser->eq_type = EQ_ASSGN;
    G_parser->undef_var = false;
    G_parser->state = PSA;
}

// ========================= PRECEDENCE SYNTAX ANALYCIS =============================

void psa_op(string_t data)
{
    orpn_add_operator(string_char_pop(data));
}

void psa_cmm(string_t data)
{
    switch (G_parser->expression)
    {
        case E_PARAM:
            orpn_add_operator(PARAM_COMMA);
            break;

        case E_MULTI_ASSGN:
            orpn_add_operator(COMMA);
            orpn_expr_end();
            ltype_insert(T_NEXT_VAL);
            break;

        case E_MULTI_FOR3:
            orpn_add_operator(COMMA);
            orpn_expr_end();
            ltype_insert(T_NEXT_VAL);
            break;

        case E_MULTI_FOR1:
            orpn_add_operator(COMMA);
            orpn_expr_end();
            ltype_insert(T_NEXT_VAL);
            break;

        case E_RETURN:
            orpn_add_operator(COMMA);
            orpn_expr_end();
            ltype_insert(T_RET_NEXT);
            break;
        
        case E_ASSGN:
            exit(SEMANTIC_OTHER_ERR); // comma in a single assign expression
            // personaly, I would use syntax error in this context
            break;

        default:
            exit(SYNTAX_ERR); // comma cannot be used in different context than obove
            break;
    }
    (void)data;
}

void psa_nl(string_t data)
{
    // if a new line follows these tokens, it is not an end of an expression
    // ad hoc fix of new lines in expression, as it is not part of the precedence syntax analysis
    if (G_parser->last_type == T_OP || G_parser->last_type == T_SEMI || G_parser->last_type == T_OB
     || G_parser->last_type == T_NL || G_parser->last_type == T_EQ || G_parser->last_type == T_FUNC
     || G_parser->last_type == T_COMMA || G_parser->last_type == T_IF)
    {
        return;
    }

    (void)data;
    switch (G_parser->expression) 
    {
        case E_RETURN:
            orpn_add_operator(RETURN_END);
            orpn_expr_end();
            ltype_insert(T_RETURN_END);
            G_parser->state = STATEMENT;
            return;

        case E_MULTI_ASSGN:
            orpn_add_operator(COND_EXPR_END);
            orpn_expr_end();
            vhtv_load_to_list_pop(G_parser->assigned_var);
            ltype_insert(T_MULTI_ASSGN_END);
            G_parser->state = STATEMENT;
            break;

        case E_ASSGN:
            orpn_add_operator(G_parser->eq_type);
            orpn_add_assgn_var(vhtv_get_last(G_parser->assigned_var));
            orpn_expr_end();
            if (get_var_state(vhtv_get_last(G_parser->assigned_var)) == V_UNDEF)
            {
                set_var_state(vhtv_get_last(G_parser->assigned_var), V_DEF);
            }
            if (lget_last_token() != T_VARS_END)
            {
                ltype_insert(T_ASSGN_END);
                lvar_insert(vhtv_pop(G_parser->assigned_var));
            }
            else
            {
                vhtv_pop(G_parser->assigned_var);
                lchange_last(T_VARS_DUMMY);
            }
            
            G_parser->state = STATEMENT;
            break;

        default:
            exit(SYNTAX_ERR); // other expressions cannot end with new line
    }
}

void psa_ocb(string_t data)
{
    (void)data;
    vu8_push(&G_parser->cb_stack, G_parser->curly_bracket);
    switch (G_parser->expression)
    {
        case E_FOR3:
            G_parser->curly_bracket = FOR_CB;
            orpn_add_operator(G_parser->eq_type);
            orpn_add_assgn_var(vhtv_get_last(G_parser->assigned_var));
            orpn_expr_end();
            ltype_insert(T_ASSGN_END);
            lvar_insert(vhtv_pop(G_parser->assigned_var));
            lfor3_end(T_FOR3_END);
            ltype_insert(T_FOR_BODY);
            break;

        case E_MULTI_FOR3:
            G_parser->curly_bracket = FOR_CB;
            orpn_add_operator(COND_EXPR_END);
            orpn_expr_end();
            vhtv_load_to_list_pop(G_parser->assigned_var);
            lfor3_end(T_MULTI_ASSGN_END);
            ltype_insert(T_FOR_BODY);
            break;

        case E_IF:
            G_parser->curly_bracket = IF_CB;
            orpn_add_operator(COND_EXPR_END);
            orpn_expr_end();
            ltype_insert(T_IF_BODY);
            break;

        case E_ELSE:
            G_parser->curly_bracket = ELSE_CB;
            ltype_insert(T_ELSE);
            break;

        case E_ELIF:
            G_parser->curly_bracket = ELIF_CB;
            orpn_add_operator(COND_EXPR_END);
            orpn_expr_end();
            ltype_insert(T_IF_BODY);
            break;

        default:
            exit(SYNTAX_ERR); // other expression cannot end with open curly bracket
    }

    start_scope(++G_parser->scope_level);
    G_parser->state = NL_NEXT;
}

void psa_cb(string_t data)
{
    if (G_parser->last_type == T_COMMA)
    {
        exit(SYNTAX_ERR); // close bracket cannot be used directly after comma
    }
    (void)data;
    if (orpn_add_cb())
    {
        G_parser->expression = vu8_pop(G_parser->expr_stack);
        if (G_parser->expression == E_FIRST_FUNC)
        {
            G_parser->state = NL_NEXT;
            G_parser->expression = vu8_pop(G_parser->expr_stack);
            orpn_add_operator(COND_EXPR_END);
            orpn_expr_end();
        }
    }
}

void psa_semi(string_t data)
{
    (void)data;
    switch (G_parser->expression)
    {
        case E_FOR1:
            orpn_add_operator(COND_EXPR_END);
            orpn_expr_end();
            G_parser->expression = E_FOR2;
            ltype_insert(T_ASSGN_END);
            lvar_insert(vhtv_get_last(G_parser->assigned_var));
            set_var_state(vhtv_pop(G_parser->assigned_var), V_DEF);
            ltype_insert(T_FOR1_END);
            ltype_insert(T_FOR2);
            break;
    
        case E_MULTI_FOR1:
            orpn_add_operator(COND_EXPR_END);
            orpn_expr_end();
            vhtv_load_to_list_pop(G_parser->assigned_var);
            ltype_insert(T_MULTI_ASSGN_END);
            ltype_insert(T_FOR2);
            G_parser->expression = E_FOR2;
            break;
    
        case E_FOR2:
            orpn_add_operator(COND_EXPR_END);
            orpn_expr_end();
            if (lget_last_token() == T_FOR2)
            {
                exit(SYNTAX_ERR); // second part of for header cannot be empty
            }
            lfor3_start(T_ASSGN);
            G_parser->state = FOR3;
            G_parser->expression = E_FOR3;
            break;
    
        default:
            exit(SYNTAX_ERR); // semicolon cannot be used in other expressions
    }
}

void psa_idf(string_t data)
{
    htab_var_t var = find_var_ht(string_read(data));
    if (var == NULL)
    {
        // firstly add dummy variable to be sure, that it is not syntax error, as it has priority
        orpn_add_var(get_tmp_var(0));
        // addition of the variable was correct, therfore semantic error
        exit(SEMANTIC_DEF_ERR); // variable was't defined yet
    }

    if (get_var_state(var) == V_UNDEF)
    {
        var = find_next_def_var(var);
        if (var == NULL)
        {
            // firstly add dummy variable to be sure, that it is not syntax error, as it has priority
            orpn_add_var(get_tmp_var(0));
            // variable is undefined (currently being define) and there are no variables from lower scopes with the same name
            exit(SEMANTIC_DEF_ERR); 
        }
    }

    if (G_parser->ongoing_for_count)
    {
        unset_var_value(var);
    }
    orpn_add_var(var);

    string_reset(data);
}

void psa_fidf(string_t data)
{
    htab_var_t var = find_var_ht(string_read(data));
    if (var != NULL)
    {
        // if the variable is currently defined, it can be defined by a function with the same name
        if (get_var_state(var) == V_UNDEF)
        {
            var = find_next_def_var(var);
            if (var != NULL)
            {
                // function call on a variable, which causes a syntax errro, 
                // variable is followed by opened bracket without operator in between
                exit(SYNTAX_ERR);     
            }
        }
        else
        {
            // function call on a variable, which causes a syntax errro, 
            // variable is followed by opened bracket without operator in between
            exit(SYNTAX_ERR);
        }
    }

    htab_func_t function = find_func_ht(string_read(data));
    if (function == NULL)
    {
        function = insert_func_ht(string_read(data));
        set_func_state(function, F_UNDEF);
    }

    add_func_call(G_parser->function, function);
    orpn_add_func(function);
    vu8_push(&G_parser->expr_stack, G_parser->expression);
    G_parser->expression = E_PARAM;
    string_reset(data);
}

void psa_str(string_t data)
{
    orpn_add_value(T_STRING, (token_data_t){.string = string_read(data)});
    string_reset(data);
}

void psa_num(string_t data)
{
    char *number = string_read(data);
    int64_t integer;
    double float64;
    errno = 0;
    switch (string_char_pop(data))
    {
    case T_INT:
        if (number[0] != '0')
        {
            integer = strtol(number, NULL, 10);
            if (errno == ERANGE && (integer == LONG_MIN || integer == LONG_MAX))
            {
                // integer literal out of range, probably lexical error, even thoug the scanner has no chance to recognize that
                exit(LEXICAL_ERR);
            }
            orpn_add_value(T_INT, (token_data_t){.integer = integer});
        }
        else
        {
            if (number[1] == 'x' || number[1] == 'X') //hexadecimal
            {
                integer = strtol(number, NULL, 16);
                if (errno == ERANGE && (integer == LONG_MIN || integer == LONG_MAX))
                {
                    // out of range
                    exit(LEXICAL_ERR);
                }
                orpn_add_value(T_INT, (token_data_t){.integer = integer});
            }
            else if (number[1] == 'b' || number[1] == 'B') //binary
            {
                integer = strtol(&number[2], NULL, 2);
                if (errno == ERANGE && (integer == LONG_MIN || integer == LONG_MAX))
                {
                    // out of range
                    exit(LEXICAL_ERR);
                }
                orpn_add_value(T_INT, (token_data_t){.integer = integer});
            }
            else if (number[1] == 'o' || number[1] == 'O') //octal
            {
                integer = strtol(&number[2], NULL, 8);
                if (errno == ERANGE && (integer == LONG_MIN || integer == LONG_MAX))
                {
                    // out of range 
                    exit(LEXICAL_ERR);
                }
                orpn_add_value(T_INT, (token_data_t){.integer = integer});
            }
            else // also octal, integer starting with zero
            {
                integer = strtol(number, NULL, 8);
                if (errno == ERANGE && (integer == LONG_MIN || integer == LONG_MAX))
                {
                    // out of range 
                    exit(LEXICAL_ERR);
                }
                orpn_add_value(T_INT, (token_data_t){.integer = integer});
            }
        }
        break;

    case T_FLOAT:
        float64 = strtod(number, NULL);
        if (errno == ERANGE && (float64 == HUGE_VAL || float64 == -HUGE_VAL || float64 == 0.0))
        {
            // out of range
            exit(LEXICAL_ERR);
        }
        orpn_add_value(T_FLOAT, (token_data_t){.float64 = float64});
        break;

    case T_BOOL:
        orpn_add_value(T_BOOL, (token_data_t){.boolean = string_char_pop(data) == TRUE ? true : false});
        break;
    }
    string_reset(data);
}

void psa_ob(string_t data)
{
    (void)data;
    orpn_add_ob(); //call of the precedence analysis
}

// ======================== FIRST PART OF FOR HEADER ===============================

void f1_id(string_t data)
{
    htab_var_t variable = find_var_ht(string_read(data));
    if (variable == NULL)
    {
        variable = insert_var_ht(string_read(data), G_parser->scope_level);
    }
    else if (!is_special_var(variable))
    {
        if (get_var_state(variable) == V_UNDEF)
        {
            exit(SEMANTIC_DEF_ERR); // repetetive use of the same undefined variable
        }
        variable = insert_var_ht(string_read(data), G_parser->scope_level);
    }
    
    vhtv_push(&G_parser->for_variables, variable); // variable declared inside for cycle

    G_parser->state = FOR1_ASSGN;
    vhtv_push(&G_parser->assigned_var, variable);

    string_reset(data);
}

void f1_semi(string_t data)
{
    (void)data;
    lchange_last(T_FOR1_EMPTY);
    ltype_insert(T_FOR2);
    G_parser->state = PSA;
    G_parser->expression = E_FOR2;
}

void f1_nl(string_t data)
{
    (void)data;
    // state remains the same
}

// ============================= FOR1 ASSIGNMENT ===================================

void f1a_eq(string_t data)
{
    G_parser->eq_type = string_char_pop(data);
    if (G_parser->eq_type != EQ_DEF)
    {
        exit(SYNTAX_ERR);
    }
    lvar_insert(vhtv_get_last(G_parser->assigned_var));
    ltype_insert(T_VARS_END);
    G_parser->undef_var = false;
    G_parser->state = PSA;
}

void f1a_comm(string_t data)
{
    (void)data;
    G_parser->expression = E_MULTI_FOR1;
    G_parser->state = FOR1_MULTI_ASSGN;
    lchange_last(T_MULTI_ASSGN);
}

// ========================== FOR1 MULTI ASSIGNMENT ==================================

void f1ma_id(string_t data)
{
    htab_var_t variable = find_var_ht(string_read(data));
    if (variable == NULL)
    {
        variable = insert_var_ht(string_read(data), G_parser->scope_level);
    }
    else if (!is_special_var(variable))
    {
        if (get_var_state(variable) == V_UNDEF)
        {
            exit(SEMANTIC_DEF_ERR); // repetetive use of the same undefined variable
        }
        variable = insert_var_ht(string_read(data), G_parser->scope_level);
    }
    
    vhtv_push(&G_parser->for_variables, variable); // variable declared inside for cycle

    G_parser->state = FOR1_MA_COMMA;
    vhtv_push(&G_parser->assigned_var, variable);

    string_reset(data);
}

void f1ma_nl(string_t data)
{
    (void)data;
    // state remains the same
}

// ======================= FOR1 MULTI ASSIGNMENT COMMA ===============================

void f1c_comm(string_t data)
{
    (void)data;
    G_parser->state = FOR1_MULTI_ASSGN;  
}

void f1c_eq(string_t data)
{
    G_parser->eq_type = string_char_pop(data);
    if (G_parser->eq_type != EQ_DEF)
    {
        exit(SYNTAX_ERR);
    }
    
    vhtv_load_to_list(G_parser->assigned_var);
    G_parser->undef_var = false;
    G_parser->state = PSA;
}

// ======================== THIRD PART OF FOR HEADER ===============================

void f3_id(string_t data)
{
    htab_var_t variable = find_var_ht(string_read(data));
    if (variable == NULL)
    {
        G_parser->undef_var = true;
    }
    else
    {
        unset_var_value(variable);
        vhtv_push(&G_parser->assigned_var, variable);
    }

    G_parser->state = FOR3_ASSGN;

    string_reset(data);
}

void f3_ocb(string_t data)
{
    (void)data;
    lfor3_empty();
    ltype_insert(T_FOR_BODY);
    vu8_push(&G_parser->cb_stack, G_parser->curly_bracket);
    start_scope(++G_parser->scope_level);
    G_parser->curly_bracket = FOR_CB;
    G_parser->state = NL_NEXT;
}

void f3_nl(string_t data)
{
    (void)data;
    // state remains the same
}

// ============================= FOR3 ASSIGNMENT ===================================

void f3a_eq(string_t data)
{
    G_parser->eq_type = string_char_pop(data);
    if (G_parser->eq_type == EQ_DEF)
    {
        exit(SYNTAX_ERR); //definition eq cannot appear in for3 expression
    }
    else if (G_parser->undef_var)
    {
        exit(SEMANTIC_DEF_ERR); //undefined variabe in for3 ecpression
    }
    lvar_insert(vhtv_get_last(G_parser->assigned_var));
    ltype_insert(T_VARS_END);
    G_parser->undef_var = false;
    G_parser->state = PSA;
}

void f3a_comm(string_t data)
{
    (void)data;
    G_parser->expression = E_MULTI_FOR3;
    G_parser->state = FOR3_MULTI_ASSGN;
    lchange_last(T_MULTI_ASSGN);
}

// ========================== FOR3 MULTI ASSIGNMENT ==================================

void f3ma_id(string_t data)
{
    htab_var_t variable = find_var_ht(string_read(data));
    if (variable == NULL)
    {
        G_parser->undef_var = true;
    }
    else
    {
        vhtv_push(&G_parser->assigned_var, variable);
        unset_var_value(variable);
    }

    G_parser->state = FOR3_MA_COMMA;

    string_reset(data);
}

void f3ma_nl(string_t data)
{
    (void)data;
    // state remains the same
}

// ======================= FOR3 MULTI ASSIGNMENT COMMA ===============================

void f3c_comm(string_t data)
{
    (void)data;
    G_parser->state = FOR3_MULTI_ASSGN;
}

void f3c_eq(string_t data)
{
    G_parser->eq_type = string_char_pop(data);
    if (G_parser->eq_type != EQ_ASSGN)
    {
        exit(SYNTAX_ERR); //definition eq cannot appear in for3 expression
    }
    else if (G_parser->undef_var)
    {
        exit(SEMANTIC_DEF_ERR); //undefined variabe in for3 expression
    }
    vhtv_load_to_list(G_parser->assigned_var);
    G_parser->undef_var = false;
    G_parser->state = PSA;
}

// ======================== FOR CYCLE VARIABLE EQUAL ===============================

void fcv_idf(string_t data)
{
    htab_var_t variable = find_var_ht(string_read(data));
    if (variable == NULL)
    {
        G_parser->undef_var = true;
        variable = insert_var_ht(string_read(data), G_parser->scope_level);

        if (G_parser->ongoing_for_count) // varaible declared iside for cycle
        {
            vhtv_push(&G_parser->for_variables, variable);
        }
        else // variable declared outside for cycle
        {
            ltype_insert(T_VAR_DEF);
            lvar_insert(variable);
        }
    }

    G_parser->state = ASSIGN;
    vhtv_push(&G_parser->assigned_var, variable);

    string_reset(data);
}

void fcv_ocb(string_t data)
{
    (void)data;
    if (G_parser->expression == E_FOR1)
    {
        exit(SYNTAX_ERR); // open curly bracket cannot follow the first expression in for header
    }

    lfor3_empty();
    ltype_insert(T_FOR_BODY);
    vu8_push(&G_parser->cb_stack, G_parser->curly_bracket);
    start_scope(++G_parser->scope_level);
    G_parser->curly_bracket = FOR_CB;
    G_parser->state = NL_NEXT;
}

void fcv_nl(string_t data)
{
    (void)data;
    if (G_parser->last_type == T_SEMI || G_parser->last_type == T_NL)
    {
        //G_parser->state = FOR_IDF;
    }
    else
    {
        exit(SYNTAX_ERR); //new line cannot follow for directly
    }
}

void fcv_semi(string_t data)
{
    if (G_parser->expression == E_FOR3)
    {
        exit(SYNTAX_ERR); // semicolon cannot be used after third part of for header
    }

    (void)data;
    lchange_last(T_FOR1_EMPTY);
    ltype_insert(T_FOR2);
    G_parser->state = PSA;
    G_parser->expression = E_FOR2;
}

// ********************* END OF PARSER STATES DEFINITIONS **************************

bool return_check_function(uint8_t coming_token)
{
    static uint8_t current_state;
    static int skipping = 1;
    static int nested = 0;
    if (coming_token == FUNCTION_WITH_RETURN)
    {
        skipping = -1;
        nested = 0;
        current_state = LOOKING;
    }
    else if (coming_token == FUNCTION_WITHOUT_RETURN)
    {
        current_state = FOUND_DEFINITIVELY;
    }

    if (coming_token == FUNCTION_BODY_END)
    {
        return current_state != SKIP_SCOPE_FOUND && current_state != FOUND_DEFINITIVELY;
    }

    switch (current_state)
    {
    case (LOOKING):
        switch (coming_token)
        {
        case (IF_START):
        case (ELIF_START):
        case (ELSE_START):
            nested++;
            break;

        case (RETURNERINO):
            if (nested > 0)
            {
                current_state = SKIP_REST_OF_BODY;
                skipping = nested;
            }
            else
                current_state = FOUND_DEFINITIVELY;
            break;
        case (IF_END):

            skipping = nested--;
            current_state = SKIP_SCOPE_LOOKING;
            break;
        case (ELSE_END):
            nested--;
            break;

        default:
            break;
        }
        break;

    case (SKIP_SCOPE_LOOKING):
        switch (coming_token)
        {
        case (IF_START):
            nested++;
            if (nested == skipping)
            {
                current_state = LOOKING;
            }
            break;

        case (ELIF_START):
        case (ELSE_START):
            nested++;
            break;

        case (ELSE_END):
            if (nested <= skipping)
                current_state = LOOKING;
            nested--;
            break;

        case (IF_END):
            if (nested < skipping)
            {
                current_state = LOOKING;
            }
            nested--;
            break;

        case (RETURNERINO):
	    if(nested == 0)
	    {
	    	current_state = FOUND_DEFINITIVELY;
		break;
	    }			    
	    else if (nested < skipping)
            {
                current_state = SKIP_REST_OF_BODY;
                skipping = nested;
            }
            break;

        default:
            break;
        }
        break;

    case (SKIP_SCOPE_FOUND):
        if (skipping == 0)
        {
            current_state = FOUND_DEFINITIVELY;
            break;
        }
        switch (coming_token)
        {
        case (IF_START):
        case (ELIF_START):
        case (ELSE_START):
            nested++;
            break;

        case (IF_END):
            if (nested == skipping)
                current_state = LOOKING;
            nested--;
            break;

        case (ELSE_END):
            nested--;
            break;

        default:
            break;
        }
        break;
    case (SKIP_REST_OF_BODY):
        switch (coming_token)
        {

        case (IF_START):
        case (ELIF_START):
        case (ELSE_START):
            nested++;
            break;

        case (IF_END):
            if (skipping == nested)
            {
                current_state = LOOKING;
            }
            nested--;
            break;

        case (ELSE_END):
            if (skipping == nested)
            {
                current_state = SKIP_SCOPE_FOUND;
                skipping = --nested;
            }
            else
                nested--;
            break;
            
        default:
            break;
        }
        break;

    default:
        break;
    }
    return false;
}

bool is_var_assignable()
{
    return G_parser->expression != E_FOR3 && G_parser->expression != E_FOR2 && G_parser->expression != E_MULTI_FOR3 &&
        G_parser->expression != E_FOR1 && G_parser->expression != E_MULTI_FOR1 && !G_parser->ongoing_for_count;
}
