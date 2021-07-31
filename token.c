
//=========================================================================================================
// File:        token.c
// Case:        VUT, FIT, IFJ, compiler project
// Date:        9. 12. 2020
// Author(s):   David Mihola, Vladislav Sokolovskii
// Contac(s):   xmihol00@stud.fit.vutbr.cz, sokol15@stud.fit.vutbr.cz
// Compiled:    gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)
// Description: File containing function definitions responsible for managing the token stream
//==========================================================================================================

#include "token_private.h"

struct
{
    bool syntax_done;
    token_t head;                /// the head of the list
    token_t tail;                /** the parser needs to ask for 2 tokens back (hopefully just 2), so the tail must point
                                    to the second last token**/
    token_t first_for1;          /// saved position before a first for cycle, where new variables used inside the cycle will be defined
    token_t insert;              /// the position, where new tokens are inserted, when a for3 is created
    token_t reader;              /// token that is currently read - will be used by the generator   
    token_t saved_reader;        /// last saved reader
    vect_token_t reader_stack;   /// vector of saved nodes (mainly for optimization)
    token_t active_partial;      /// active node of the partail list
    vect_ll_couple_t partial_lists; /** vector of saved partial list (mainly for the 3rd parts of for statements,
    which has to be generated after the for body but in the user program it appears in the for statement - before the for body)**/
}G_token = {0, };

void init_list()
{
    token_t last_token = malloc(sizeof(struct token));     //empty node
    if (last_token == NULL) 
    {
        exit(MEM_ERR);
    }
    token_t last_2nd_token = malloc(sizeof(struct token));//empty node
    if (last_2nd_token == NULL)
    {
        exit(MEM_ERR);
    }
    
    last_token->type = NONE;
    last_2nd_token->type = NONE;
    last_token->next = NULL;
    last_2nd_token->next = last_token;
    G_token.tail = last_2nd_token;
    G_token.head = G_token.tail;

    G_token.reader_stack = vect_token_const();
    G_token.partial_lists = vect_ll_couple_const(); //vector for token couples that contain for3start and for3end pointers
    vllc_push_fisrt(&G_token.partial_lists, NULL);
}

void dispose_list()
{
    token_t tmp;
    while (G_token.head != NULL) {
        tmp = G_token.head;
        G_token.head = G_token.head->next;

        if (tmp->type == T_STRING) {
            free(tmp->data.string);
        }
        free(tmp);
    }
    G_token.tail = NULL;

    if (G_token.syntax_done)
    {
        for (int64_t i = vllc_pos(G_token.partial_lists); i >= 0; i--)
        {
            vllc_pop_couple(G_token.partial_lists, &G_token.active_partial, &G_token.tail);
            while (G_token.active_partial != NULL)
            {
                tmp = G_token.active_partial;
                G_token.active_partial = G_token.active_partial->next;
                free(tmp);
            }
        }
    }

    discard_ll_couple_vector(&G_token.partial_lists);
    discard_token_vector(&G_token.reader_stack);
}

void syntax_done()
{
    G_token.syntax_done = true;
}

void ltype_insert(uint8_t type)
{
    token_t new_token = malloc(sizeof(struct token));
    if (new_token == NULL) {
        exit(MEM_ERR);
    }
    new_token->type = type;
    new_token->next = NULL;

    G_token.tail->next->next = new_token; //move the tail
    G_token.tail = G_token.tail->next;
}

void lint_insert(int64_t integer)
{
    token_t new_token = malloc(sizeof(struct token));
    if (new_token == NULL) {
        exit(MEM_ERR);
    }
    new_token->type = T_INT;
    new_token->data.integer = integer;
    new_token->next = NULL;


    G_token.tail->next->next = new_token;
    G_token.tail = G_token.tail->next;
}

void lfloat_insert(double decimal)
{
    token_t new_token = malloc(sizeof(struct token));
    if (new_token == NULL) {
        exit(MEM_ERR);
    }
    new_token->type = T_FLOAT;
    new_token->data.float64 = decimal;
    new_token->next = NULL;

    G_token.tail->next->next = new_token;
    G_token.tail = G_token.tail->next;
}

void lbool_insert(bool boolean)
{
    token_t new_token = malloc(sizeof(struct token));
    if (new_token == NULL) {
        exit(MEM_ERR);
    }
    new_token->type = T_BOOL;
    new_token->data.boolean = boolean;
    new_token->next = NULL;

    G_token.tail->next->next = new_token;
    G_token.tail = G_token.tail->next;
}

void lstring_insert(char *string)
{
    token_t new_token = malloc(sizeof(struct token));
    if (new_token == NULL) {
        exit(MEM_ERR);
    }

    new_token->type = T_STRING;
    new_token->data.string = string;
    new_token->next = NULL;

    G_token.tail->next->next = new_token;
    G_token.tail = G_token.tail->next;
}

void lvar_insert(htab_var_t variable)
{
    token_t new_token = malloc(sizeof(struct token));
    if (new_token == NULL) {
        exit(MEM_ERR);
    }
    new_token->type = T_IDF;
    new_token->data.var = variable;
    new_token->next = NULL;

    G_token.tail->next->next = new_token;
    G_token.tail = G_token.tail->next;
}

void lfunc_insert(htab_func_t function)
{
    token_t new_token = malloc(sizeof(struct token));
    if (new_token == NULL) {
        exit(MEM_ERR);
    }
    new_token->type = T_FUNC;
    new_token->data.func = function;
    new_token->next = NULL;

    G_token.tail->next->next = new_token;
    G_token.tail = G_token.tail->next;
}

void lop_insert(uint8_t operator)
{
    token_t new_token = malloc(sizeof(struct token));
    if (new_token == NULL) {
        exit(MEM_ERR);
    }

    new_token->type = T_OP;
    new_token->data.op = operator;
    new_token->next = NULL;

    G_token.tail->next->next = new_token;
    G_token.tail = G_token.tail->next;
}

uint8_t lget_last_token()
{
    return G_token.tail->next->type;
}

uint8_t lget_2nd_last_token()
{
    return G_token.tail->type;
}

void lreader_save_pos()
{
    G_token.saved_reader = G_token.reader;
}

void lreader_head()
{
    G_token.reader = G_token.head->next->next;
}

void lreader_pre_head()
{
    G_token.reader = G_token.head->next;
}

void lreader_next()
{
    G_token.reader = G_token.reader->next;
}

void lreader_change_type(uint8_t type)
{
    G_token.reader->type = type;
}

void lreader_change_n_type(uint8_t type)
{
    G_token.reader->next->type = type;
}

void lreader_change_n_data(token_data_t data)
{
    G_token.reader->next->data = data;
}

uint8_t lreader_type()
{
    return G_token.reader->type;
}

uint8_t lreader_n_type()
{
    return G_token.reader->next->type;
}

bool lreader_find_occurence_until(uint8_t type, uint8_t until)
{
    token_t tmp = G_token.reader;
    while (G_token.reader->type != until)
    {
        if (G_token.reader->type == type)
        {
            G_token.reader = tmp;
            return true;        
        }
        G_token.reader = G_token.reader->next;
    }
    G_token.reader = tmp;
    return false;
}

bool lreader_find_var_until(htab_var_t var, uint8_t until)
{
    //token_t tmp = G_token.reader;
    G_token.reader = G_token.reader->next;
    while (G_token.reader->type != until)
    {
        if (G_token.reader->type == T_IDF && G_token.reader->data.var == var)
        {
            //G_token.reader = tmp;
            return true;        
        }
        G_token.reader = G_token.reader->next;
    }
    //G_token.reader = tmp;
    return false;
}

uint8_t lreader_move_next_type()
{
    G_token.reader = G_token.reader->next;
    return G_token.reader->next->type;
}

uint8_t lreader_nn_type()
{
    return G_token.reader->next->next->type;
}

token_data_t lreader_data()
{
    return G_token.reader->data;
}

token_data_t lreader_n_data()
{
    return G_token.reader->next->data;
}

token_data_t lreader_nn_data()
{
    return G_token.reader->next->next->data;
}

token_data_t lreader_nnn_data()
{
    return G_token.reader->next->next->next->data;
}

token_t lreader_n_token()
{
    return G_token.reader->next;
}

token_t lreader_token()
{
    return G_token.reader;
}

void lreader_insert_construct(uint8_t type, token_data_t data)
{
    token_t tmp = malloc(sizeof(struct token));
    if (tmp == NULL)
    {
        exit(MEM_ERR);
    }
    tmp->data = G_token.reader->data;
    tmp->type = G_token.reader->type;
    G_token.reader->data = data;
    G_token.reader->type = type;
    tmp->next = G_token.reader->next;
    G_token.reader->next = tmp;
    G_token.reader = G_token.reader->next;
}

void lreader_n_insert_construct(uint8_t type, token_data_t data)
{
    token_t tmp = malloc(sizeof(struct token));
    if (tmp == NULL)
    {
        exit(MEM_ERR);
    }
    tmp->data = data;
    tmp->type = type;
    tmp->next = G_token.reader->next->next;
    G_token.reader->next = tmp;
}

void lreader_n_replace(uint8_t type, token_data_t data)
{
    G_token.reader->next->data = data;
    G_token.reader->next->type = type;
}

void lreader_save_pos_to_stack()
{
    vtn_push(&G_token.reader_stack, G_token.reader);
}

void lreader_load_pos_from_stack()
{
    G_token.reader = vtn_pop(G_token.reader_stack);
}

void lreader_pop_stack()
{
    vtn_pop(G_token.reader_stack);
}

void lreader_reaload_saved_pos()
{
    token_t tmp = G_token.reader;
    G_token.reader = G_token.saved_reader;
    G_token.saved_reader = tmp;
}

void lsaved_pos_insert_construct(uint8_t type, token_data_t data)
{
    token_t tmp = malloc(sizeof(struct token));
    if (tmp == NULL)
    {
        exit(MEM_ERR);
    }
    tmp->data = data;
    tmp->type = type;
    tmp->next = G_token.saved_reader->next;
    G_token.saved_reader->next = tmp;
    G_token.saved_reader = G_token.saved_reader->next;
}

void lsaved_pos_insert(token_t token)
{
    token->next = G_token.saved_reader->next;
    G_token.saved_reader->next = token;
    G_token.saved_reader = G_token.saved_reader->next;
}

void lreader_remove_until(uint8_t type)
{
    token_t tmp;
    while(G_token.reader->next->type != type)
    {
        tmp = G_token.reader->next;
        G_token.reader->next = tmp->next;
        free(tmp);
    }
    if (type == T_ASSGN_END)
    {
        G_token.reader = G_token.reader->next;
        G_token.reader->type = T_ADR_ASSGN_END;
        tmp = G_token.reader->next;
        G_token.reader->next = tmp->next;
        free(tmp);
    }
}

void lsaved_remove_until(uint8_t type)
{
    token_t tmp;
    while(G_token.saved_reader->next->type != type)
    {
        tmp = G_token.saved_reader->next;
        G_token.saved_reader->next = tmp->next;
        free(tmp);
    }
    if (type == T_ASSGN_END)
    {
        G_token.saved_reader = G_token.saved_reader->next;
        G_token.saved_reader->type = T_ADR_ASSGN_END;
        tmp = G_token.saved_reader->next;
        G_token.saved_reader->next = tmp->next;
        free(tmp);
    }
}

void lreader_remove_next()
{
    token_t tmp = G_token.reader->next->next;
    free(G_token.reader->next);
    G_token.reader->next = tmp;
}

void lreader_remove()
{
    token_t tmp = G_token.reader->next->next;
    G_token.reader->data = G_token.reader->next->data;
    G_token.reader->type = G_token.reader->next->type;
    free(G_token.reader->next);
    G_token.reader->next = tmp;
}

bool lreader_remove_func_def()
{
    bool ret_val;
    token_t tmp = G_token.reader->next->next;
    G_token.reader->data = G_token.reader->next->data;
    free(G_token.reader->next);
    G_token.reader->type = T_EMPTY;
    G_token.reader->next = tmp;
    ret_val = tmp->type == T_FCALL_END;
    G_token.reader->next->type = T_PARAM;
    return ret_val;
}

void lreader_skip_until(uint8_t type)
{
    while (G_token.reader->next->type !=  type)
    {
        G_token.reader = G_token.reader->next;
    }
}

void lreader_remove_next_two()
{
    token_t tmp = G_token.reader->next->next->next;
    free(G_token.reader->next->next);
    free(G_token.reader->next);
    G_token.reader->next = tmp;
}

void lreader_insert_next(uint8_t type, token_data_t data)
{
    token_t tmp = malloc(sizeof(struct token));
    if (tmp == NULL)
    {
        exit(MEM_ERR);
    }
    tmp->data = data;
    tmp->type = type;
    tmp->next = G_token.reader->next->next;
    G_token.reader->next->next = tmp;
    G_token.reader = G_token.reader->next;
}

uint8_t lreader_find_next_data_type()
{
    vect_uint8_t ret_vals;
    token_t tmp = G_token.reader->next->next;
    while (tmp->type != T_INT && tmp->type != T_FLOAT && tmp->type != T_BOOL && tmp->type != T_FUNC && tmp->type != T_IDF && tmp->type != T_STRING)
    {
        tmp = tmp->next;
    }
    switch(tmp->type)
    {
        case T_INT:
            return T_INT;

        case T_FLOAT:
            return T_FLOAT;

        case T_BOOL:
            return T_BOOL;

        case T_STRING:
            return T_STRING;
        
        case T_IDF:
            return get_var_type(tmp->data.var);
        
        case T_FUNC:
            ret_vals = get_ret_val_types(tmp->data.func);
            if (ret_vals == NULL)
            {
                return 0;
            }
            return vu8_get_last(ret_vals);
    }
    return 0;
}

bool llist_is_not_last()
{
    return G_token.reader->next;
}

token_t lread_token_next()
{
    token_t tmp = G_token.reader;
    G_token.reader = G_token.reader->next;
    return tmp;
}

bool llist_is_not_empty()
{
    return G_token.reader;
}

void lreader_saved_pos()
{
    G_token.reader = vtn_pop(G_token.reader_stack);
}

void lpartial_pop_free()
{
    vllc_pop_couple(G_token.partial_lists, &G_token.first_for1, &G_token.insert);
    while(G_token.first_for1 != NULL)
    {
        G_token.insert = G_token.first_for1;
        G_token.first_for1 = G_token.first_for1->next;
        free(G_token.insert);
    }
    G_token.active_partial = vllc_get_last_second(G_token.partial_lists);
}

void lpartial_create()
{
    vllc_add_second(G_token.partial_lists, G_token.active_partial);
    G_token.active_partial = malloc(sizeof(struct token));
    if (G_token.active_partial == NULL)
    {
        exit(MEM_ERR);
    }
    G_token.active_partial->type = T_EMPTY;
    G_token.active_partial->next = NULL;
    vllc_push_fisrt(&G_token.partial_lists, G_token.active_partial);
}

void lpartial_insert_create(uint8_t type, token_data_t data)
{
    token_t tmp = malloc(sizeof(struct token));
    if (tmp == NULL)
    {
        exit(MEM_ERR);
    }
    tmp->data = data;
    tmp->type = type;
    if (G_token.active_partial->next == NULL)
    {
        G_token.active_partial->next = tmp;
        G_token.active_partial = G_token.active_partial->next;
        G_token.active_partial->next = NULL;

    }
    else
    {
        tmp->next = G_token.active_partial->next;
        G_token.active_partial->next = tmp;
        G_token.active_partial = G_token.active_partial->next;
    }
}

void lpartial_insert_create_last(uint8_t type, token_data_t data)
{
    while(G_token.active_partial->next != NULL)
    {
        G_token.active_partial = G_token.active_partial->next;
    }
    G_token.active_partial->next = malloc(sizeof(struct token));
    if (G_token.active_partial->next == NULL)
    {
        exit(MEM_ERR);
    }
    G_token.active_partial = G_token.active_partial->next;
    G_token.active_partial->data = data;
    G_token.active_partial->type = type;
    G_token.active_partial->next = NULL;
}

void lpartial_insert_create_after(uint8_t type, token_data_t data)
{
    token_t tmp = malloc(sizeof(struct token));
    if (tmp == NULL)
    {
        exit(MEM_ERR);
    }
    tmp->data = data;
    tmp->type = type;
    if (G_token.active_partial->next == NULL)
    {
        G_token.active_partial->next = tmp;
        tmp->next = NULL;

    }
    else
    {
        tmp->next = G_token.active_partial->next;
        G_token.active_partial->next = tmp;
    }
}

void lpartial_insert(token_t token)
{
    if (G_token.active_partial->next == NULL)
    {
        G_token.active_partial->next = token;
        G_token.active_partial = G_token.active_partial->next;
        G_token.active_partial->next = NULL;
    }
    else
    {
        token->next = G_token.active_partial->next;
        G_token.active_partial->next = token;
        G_token.active_partial = G_token.active_partial->next;
    }
}

void lpartial_insert_partial_to_reader()
{
    vllc_pop_couple(G_token.partial_lists, &G_token.first_for1, &G_token.insert);
    while(G_token.active_partial->next != NULL)
    {
        G_token.active_partial = G_token.active_partial->next;
    }
    G_token.active_partial->next = G_token.reader->next;
    G_token.reader->next = G_token.first_for1->next;
    free(G_token.first_for1);
    G_token.reader = G_token.active_partial;
    G_token.active_partial = vllc_get_last_second(G_token.partial_lists);
}

void lpartial_change_type(uint8_t type)
{
    G_token.active_partial->type = type;
}

token_data_t lcheck_next(uint8_t *type)
{
    if (type != NULL) {
        *type = G_token.reader->type;
    }
    return G_token.reader->data;
}

token_data_t lread_next(uint8_t *type)
{
    token_data_t tmp = G_token.reader->data;
    *type = G_token.reader->type;
    G_token.reader = G_token.reader->next;
    return tmp;
}

token_t tcreate_token(uint8_t type, token_data_t data)
{
    token_t tmp = malloc(sizeof(struct token));
    if (tmp == NULL)
    {
        exit(MEM_ERR);
    }
    tmp->type = type;
    tmp->data = data;
    return tmp;
}

token_t tcopy_token(token_t token)
{
    token_t tmp = malloc(sizeof(struct token));
    if (tmp == NULL)
    {
        exit(MEM_ERR);
    }
    tmp->data = token->data;
    tmp->type = token->type;
    return tmp;
}

token_data_t tget_token_data(token_t token)
{
    return token->data;
}

uint8_t tget_token_type(token_t token)
{
    return token->type;
}

void lfor3_start(uint8_t type)
{
    ltype_insert(type);
    vllc_push_fisrt(&G_token.partial_lists, G_token.tail);
}

void lfor3_end(uint8_t type)
{
    ltype_insert(type);
    vllc_add_second(G_token.partial_lists, G_token.tail);
}

void lfor3_empty()
{
    ltype_insert(T_FOR3_EMPTY);
    vllc_add_second(G_token.partial_lists, NULL);
}

void lfor_body_end(uint8_t type)
{
    token_t  fStart, fEnd, tmp;
    vllc_pop_couple(G_token.partial_lists, &fStart, &fEnd);
    if (fEnd == NULL)
    {
        tmp = fStart->next;
        fStart->next = tmp->next->next;
        G_token.tail->next->next = tmp->next;
        G_token.tail = G_token.tail->next;
        G_token.tail->next->next = NULL;
        free(tmp);
    }
    else
    {
        tmp = fStart->next; //saves the position of the beginning of the FOR3 part
        fStart->next = fEnd->next->next; // connects the last token of the FOR2 part with the beginning of the FOR_BODY
        G_token.tail->next->next = tmp; //moves the FOR3 part to the end of the for body
        G_token.tail = fEnd;
        fEnd->next->next = NULL; //connects the last token of the FOR3 part with the FOR_BODY_END
    }

    ltype_insert(type);
}

void save_first_for()
{
    G_token.first_for1 = G_token.tail;
}

void store_for_variables(vect_htab_var_t vars)
{
    token_t tmp = malloc(sizeof(struct token));
    if (tmp == NULL)
    {
        exit (MEM_ERR);
    }
    token_t discard = tmp;
    while(vhtv_pos(vars) >= 0)
    {
        tmp->next = malloc(sizeof(struct token));
        if (tmp->next == NULL)
        {
            exit (MEM_ERR);
        }
        tmp->next->type = T_VAR_DEF;
        tmp = tmp->next;
        tmp->next = malloc(sizeof(struct token));
        if (tmp->next == NULL)
        {
            exit (MEM_ERR);
        }
        tmp->next->type = T_IDF;
        tmp->next->data.var = vhtv_pop(vars);
        tmp = tmp->next;
    }
    tmp->next = G_token.first_for1->next;
    G_token.first_for1->next = discard->next;
    
    free(discard);
}

void print_list()
{
    char expr_data[][128] = {[18] = "EXPRESSION_END", "ASSIGN_START", "MULTI_ASSIGN", "ASSIGN_END", "MULTI_ASSIGN_END", 
    "FUN_DEF", "FIRST_FUNC_CALL", "FIRST_FCALL_END", "FUNC_CALL", "FUN_CALL_END", "PARAM", "PARAM_END", "IF", "IF_BODY", 
    "IF_BODY_END", "ELSE", "ELSE_BODY", "ELSE_BODY_END", "ELIF", "ELIF_BODY", "ELIF_BODY_END", "FOR1", 
    "FOR1_EMPTY", "FOR1_END", "FOR2", "FOR3", "FOR3_EMPTY", "FOR3_END", "FOR_BODY", "FOR_BODY_END", "FUNC_BODY", 
    "FUNC_BODY_END", "RETURN", "RETURN_NEXT", "RETURN_END", "STATEMENT", "NEXT_VAL", "VAR_DEF", "JMP_IF_EQ", "JMP_IF_NEQ", 
    "JMP_FOR_EQ", "JMP_FOR_NEQ", "REG", "ADRESS_ASSGN", "ADR_ASSGN_END", "MOV", "SINGLE_VAL_IF", "SINGLE_VAL_FOR",
    "VARS_END", "WRITE", "LEN", "INT2FLOAT", "FLOAT2INT", "PUSHS", "PRECOMPILED", "T_VARS_DUMMY",
    "EMPTY", "INPUTS", "INPUTI", "INPUTF", "INPUTB", "INT2FLOAT", "PRINT", "ORD", "CHR", 
    "SUBSTR", "LENGHT"};

    char tokens_data[][128] = {[11] = "true", "false", "NEG", "POS", "!", "!!", [IDIV] "/", "ZERO",
     [0x2e] = "*", "/", [0x40] = "+", "-", [0x60] = "<", "<=", ">", ">=", "==", "!=", [0x80] = "&&",\
     [0xa0] = "||", [0xc0] = "+=", "*=", "/=", "-=", "=", ":="};

    token_t iterator = G_token.head->next->next;
    while (iterator != NULL) {
        switch (iterator->type) {
            case T_STRING:
                fprintf(stderr, "\"%s\" ", iterator->data.string);
                break;
            case T_INT:
                fprintf(stderr, "%ld ", iterator->data.integer);
                break;
            case T_FLOAT:
                fprintf(stderr, "%lf ", iterator->data.float64);
                break;

            case T_REGISTER:
                fprintf(stderr, "R: %s ", iterator->data.string);
                break;

            case T_EQ:
            case T_OP:
                fprintf(stderr, "OP: %s ", tokens_data[iterator->data.op]);
                break;
            case T_IDF:
                fprintf(stderr, "V: %s ", read_var_name(iterator->data.var));
                break;

            case T_FUNC:
                fprintf(stderr, "F: %s ", read_func_name(iterator->data.func));
                break;
            
            case T_BOOL:
                fprintf(stderr, "%s ", iterator->data.boolean ? "true" : "false");
                break;

            default:
                fprintf(stderr, "|%s| ", expr_data[iterator->type]);
                break;
        }
        iterator = iterator->next;
    }
    fprintf(stderr, "\n");
}

void lchange_last(uint8_t type)
{
    G_token.tail->next->type = type;
}
