
//=========================================================================================================
// File:        vector.c
// Case:        VUT, FIT, IFJ, compiler project
// Date:        9. 12. 2020
// Author(s):   David Mihola
// Contac(s):   xmihol00@stud.fit.vutbr.cz
// Compiled:    gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)
// Description: File containing defintions of functions operating on auxiliary data types.
//==========================================================================================================

#include "vector_private.h"


// ======================================== uint8_vector START ========================================

vect_uint8_t vect_uint8_const()
{
    T_vect_uint8 *tmp = calloc(1, sizeof(uint8_t)*VECTOR_START_SIZE + sizeof(T_vect_uint8));
    if (tmp == NULL)
        exit(MEM_ERR);
    
    tmp->pos = VECTOR_START_POS;
    tmp->size = VECTOR_START_SIZE;

    MMU_add(tmp);

    return (vect_uint8_t)tmp;
}

int64_t vu8_pos(vect_uint8_t vector)
{
    return vector->pos;
}

void vu8_set_pos(vect_uint8_t vector, int64_t pos, uint8_t value)
{
    vector->values[pos] = value;
}

void vu8_set_offset(vect_uint8_t vector, int64_t offset, uint8_t value)
{
    vector->values[vector->pos - offset] = value;
}

void vu8_push(vect_uint8_t *vector, uint8_t value)
{
    vect_uint8_t tmp = *vector;
    if (++tmp->pos == tmp->size)
    {
        tmp->size *= 2;
        *vector = tmp = MMU_realloc_item(tmp, tmp->size * sizeof(uint8_t) + sizeof(T_vect_uint8));
    }
    tmp->values[tmp->pos] = value;
}

void vu8_push_under(vect_uint8_t *vector, uint8_t value)
{
    vect_uint8_t tmp = *vector;
    if (++tmp->pos == tmp->size)
    {
        tmp->size *= 2;
        *vector = tmp = MMU_realloc_item(tmp, tmp->size * sizeof(uint8_t) + sizeof(T_vect_uint8));
    }
    tmp->values[tmp->pos] = tmp->values[tmp->pos - 1];
    tmp->values[tmp->pos - 1] = value;
}

void vu8_multi_pop(vect_uint8_t vector, int64_t count)
{
    vector->pos -= count + 1;
}

void vu8_multi_pop_under(vect_uint8_t vector, int64_t count)
{
    vector->values[vector->pos - count - 1] = vector->values[vector->pos];
    vector->pos -= count + 1;
}

uint8_t vu8_get_pos(vect_uint8_t vector, int64_t pos)
{
    return vector->values[pos];
}

uint8_t vu8_get_last(vect_uint8_t vector)
{
    return vector->values[vector->pos];
}

void vu8_set_last(vect_uint8_t vector, uint8_t value)
{
    vector->values[vector->pos] = value;
}

uint8_t vu8_get_offset(vect_uint8_t vector, int64_t offset)
{
    return vector->values[vector->pos - offset];
}

uint8_t vu8_pop(vect_uint8_t vector)
{
    return vector->values[vector->pos--];
}

bool vu8_is_empty(vect_uint8_t vector)
{
    return vector->pos == -1;
}

void vu8_reset(vect_uint8_t vector)
{
    vector->pos = VECTOR_START_POS;
}

void discard_uint8_vector(vect_uint8_t *vector)
{
    MMU_discard(*vector);
    *vector = NULL;
}

// ========================================= uint8_vector END =========================================

// ======================================== uint32_vector START ========================================

vect_int32_t vect_int32_const()
{
    T_vect_int32 *tmp = calloc(1, sizeof(int32_t)*VECTOR_START_SIZE + sizeof(T_vect_int32));
    if (tmp == NULL)
        exit(MEM_ERR);
    
    tmp->pos = VECTOR_START_POS;
    tmp->size = VECTOR_START_SIZE;

    MMU_add(tmp);

    return (vect_int32_t)tmp;
}

void vi32_push(vect_int32_t *vector, int32_t value)
{
    vect_int32_t tmp = *vector;
    if (++tmp->pos == tmp->size)
    {
        tmp->size *= 2;
        *vector = tmp = MMU_realloc_item(tmp, tmp->size * sizeof(int32_t) + sizeof(T_vect_int32));
    }
    tmp->values[tmp->pos] = value;
}

int32_t vi32_get_last(vect_int32_t vector)
{
    return vector->values[vector->pos];
}

int32_t vi32_get_offset(vect_int32_t vector, int64_t offset)
{
    return vector->values[vector->pos - offset];
}

int32_t vi32_pop(vect_int32_t vector)
{
    return vector->values[vector->pos--];
}

void vect_int32_reset(vect_int32_t vector)
{
    vector->pos = VECTOR_START_POS;
}

void discard_int32_vector(vect_int32_t *vector)
{
    MMU_discard(*vector);
    *vector = NULL;
}

// ========================================= uint32_vector END =========================================

// ======================================== string_vector START =========================================

vect_string_t vect_str_const()
{
    T_vect_string *tmp = calloc(1, sizeof(T_vect_string) + VECTOR_START_SIZE * sizeof(string_t));
    if (tmp == NULL)
        exit(MEM_ERR);
    
    tmp->pos = VECTOR_START_POS;
    tmp->size = VECTOR_START_SIZE;

    MMU_add(tmp);
    return (vect_string_t)tmp;
}

void vstr_push(vect_string_t *vector, string_t value)
{
    vect_string_t tmp = *vector;
    if (++tmp->pos == tmp->size)
    {
        tmp->size *= 2;
        *vector = tmp = MMU_realloc_item(tmp, tmp->size * sizeof(string_t) + sizeof(T_vect_string));
    }
    tmp->values[tmp->pos] = value;
}

string_t vstr_get_last(vect_string_t vector)
{
    return vector->values[vector->pos];
}

string_t vstr_get_offset(vect_string_t vector, int64_t offset)
{
    return vector->values[vector->pos - offset];
}

string_t vstr_pop(vect_string_t vector)
{
    return vector->values[vector->pos--];
}

void vstr_pop_free(vect_string_t vector)
{
    MMU_discard(vector->values[vector->pos]);
}

void vstr_reset(vect_string_t vector)
{
    vector->pos = VECTOR_START_POS;
}

void vstr_reset_free(vect_string_t vector)
{
    for (int64_t i = 0; i < vector->pos; i++)
        MMU_discard(vector->values[i]);

    vector->pos = VECTOR_START_POS;
}

void discard_str_vector(vect_string_t *vector)
{
    vstr_reset(*vector);
    MMU_discard(*vector);
    *vector = NULL;
}

void discard_str_vector_free(vect_string_t *vector)
{
    vstr_reset_free(*vector);
    MMU_discard(*vector);
    *vector = NULL;
}

// ========================================= string_vector END ==========================================

// ========================================== var_vector START ==========================================

vect_htab_var_t vect_htab_var_const()
{
    T_vect_var *tmp = calloc(1, sizeof(T_vect_var) + VECTOR_START_SIZE * sizeof(htab_var_t));
    if (tmp == NULL)
        exit(MEM_ERR);
    
    tmp->pos = VECTOR_START_POS;
    tmp->size = VECTOR_START_SIZE;

    MMU_add(tmp);
    return (vect_htab_var_t)tmp;
}

void vhtv_push(vect_htab_var_t *vector, htab_var_t value)
{
    vect_htab_var_t tmp = *vector;
    if (++tmp->pos == tmp->size)
    {
        tmp->size *= 2;
        *vector = tmp = MMU_realloc_item(tmp, tmp->size * sizeof(htab_var_t) + sizeof(T_vect_var));
    }
    tmp->values[tmp->pos] = value;
}

htab_var_t vhtv_get_last(vect_htab_var_t vector)
{
    return vector->values[vector->pos];
}

htab_var_t vhtv_get_offset(vect_htab_var_t vector, int64_t offset)
{
    return vector->values[vector->pos - offset];
}

htab_var_t vhtv_get_pos(vect_htab_var_t vector, int64_t position)
{
    return vector->values[position];
}

int64_t vhtv_pos(vect_htab_var_t vector)
{
    return vector->pos;
}

htab_var_t vhtv_pop(vect_htab_var_t vector)
{
    return vector->values[vector->pos--];
}

void vhtv_load_to_list_pop(vect_htab_var_t vector)
{
    for (int64_t i = vector->pos; i >= 0; i--)
    {
        ltype_insert(T_ASSGN_END);
        lvar_insert(vector->values[i]);
        unset_var_value(vector->values[i]);
        if (get_var_state(vector->values[i]) == V_UNDEF)
        {
            set_var_state(vector->values[i], V_DEF);
        }
    }
    vector->pos = -1;
}

void vhtv_load_to_list(vect_htab_var_t vector)
{
    for (int64_t i = vector->pos; i >= 0; i--)
    {
        lvar_insert(vector->values[i]);
    }
    ltype_insert(T_VARS_END);
}

void vhtv_pop_free(vect_htab_var_t vector)
{
    MMU_discard(vector->values[vector->size--]);
}

void vhtv_reset(vect_htab_var_t vector)
{
    vector->pos = VECTOR_START_POS;
}

void vhtv_reset_free(vect_htab_var_t vector)
{
    for (int64_t i = 0; i <= vector->pos; i++)
        MMU_discard(vector->values[i]);
    
    vector->pos = VECTOR_START_POS;
}

void discard_htab_var_vector(vect_htab_var_t *vector)
{
    MMU_discard(*vector);
    *vector = NULL;
}

void discard_htab_var_vector_free(vect_htab_var_t *vector)
{
    vhtv_reset_free(*vector);
    MMU_discard(*vector);
    *vector = NULL;
}

bool vhtv_is_empty(vect_htab_var_t vector)
{
    return vector->pos == -1;
}


// ========================================== var_vector END ============================================

// ========================================= func_vector START ==========================================

vect_htab_func_t vect_htab_func_const()
{
    T_vect_func *tmp = calloc(1, sizeof(T_vect_func) + VECTOR_START_SIZE * sizeof(htab_func_t));
    if (tmp == NULL)
        exit(MEM_ERR);
    
    tmp->pos = VECTOR_START_POS;
    tmp->size = VECTOR_START_SIZE;

    MMU_add(tmp);
    return (vect_htab_func_t)tmp;
}

void vfht_push(vect_htab_func_t *vector, htab_func_t value)
{
    vect_htab_func_t tmp = *vector;
    if (++tmp->pos == tmp->size)
    {
        tmp->size *= 2;
        *vector = tmp = MMU_realloc_item(tmp, tmp->size * sizeof(htab_func_t) + sizeof(T_vect_func));
    }
    tmp->values[tmp->pos] = value;
}

void vfht_push_unique(vect_htab_func_t *vector, htab_func_t value)
{
    vect_htab_func_t tmp = *vector;
    for (int64_t i = tmp->pos; i >= 0; i--)
    {
        if (tmp->values[i] == value)
        {
            return;
        }
    }

    if (++tmp->pos == tmp->size)
    {
        tmp->size *= 2;
        *vector = tmp = MMU_realloc_item(tmp, tmp->size * sizeof(htab_func_t) + sizeof(T_vect_func));
    }
    tmp->values[tmp->pos] = value;
}

htab_func_t vfht_get_last(vect_htab_func_t vector)
{
    return vector->values[vector->pos];
}

htab_func_t vfht_get_offset(vect_htab_func_t vector, int64_t offset)
{
    return vector->values[vector->pos - offset];
}

int64_t vfht_pos(vect_htab_func_t vector)
{
    return vector->pos;
}

htab_func_t vfht_pop(vect_htab_func_t vector)
{
    return vector->values[vector->pos--];
}

void vfht_reset(vect_htab_func_t vector)
{
    vector->pos = VECTOR_START_POS;
}

void discard_htab_func_vector(vect_htab_func_t *vector)
{
    MMU_discard(*vector);
    *vector = NULL;
}

// ========================================== func_vector END ===========================================

// =========================================== string START ============================================

string_t string_const()
{
    T_string *tmp = calloc(1, sizeof(T_string) + STRING_START_SIZE * sizeof(char));
    if (tmp == NULL)
    {
        exit(MEM_ERR);
    }
    tmp->pos = STRING_START_POS;
    tmp->size = STRING_START_SIZE - STRING_SAFETY_OFFSET;
    MMU_add(tmp);

    return tmp;
}

void string_char_push(string_t *str, char value)
{
    T_string *tmp = *str;
    if (++tmp->pos == tmp->size)
    {
        tmp->size += STRING_SAFETY_OFFSET;
        tmp->size *= 2;
        *str = tmp = MMU_realloc_item(tmp, tmp->size * sizeof(char) + sizeof(T_string));
        tmp->size -= STRING_SAFETY_OFFSET;
    }
    tmp->str[tmp->pos] = value;
}

char string_get_last(string_t str)
{
    return str->str[str->pos];
}

char string_char_pop(string_t str)
{
    return str->str[str->pos--];   
}

void string_substr_push(string_t *str, char *substr, int64_t lenght)
{
    T_string *tmp = *str;
    if (tmp->pos + lenght >= tmp->size)
    {
        tmp->size += STRING_SAFETY_OFFSET;
        tmp->size *= 2;
        *str = tmp = MMU_realloc_item(tmp, tmp->size * sizeof(char) + sizeof(T_string));
        tmp->size -= STRING_SAFETY_OFFSET;
    }

    strncpy(&tmp->str[++tmp->pos], substr, lenght);
    tmp->pos += lenght - 1;
}

char *string_read(string_t str)
{   
    str->str[str->pos + 1] = '\0';
    return str->str;
}

void string_concat(string_t *str_dest, string_t str_src)
{
    string_substr_push(str_dest, str_src->str, str_src->pos + 1);
}

void string_rewrite(string_t *str_dest, string_t str_src)
{
    (*str_dest)->pos = -1;
    string_substr_push(str_dest, str_src->str, str_src->pos + 1);
}

void string_reset(string_t str)
{
    str->pos = STRING_START_POS;
}

void discard_string(string_t *str)
{
    MMU_discard(*str);
    *str = NULL;
}

char* string_get_buffer(struct string *str)
{
    return str->str;
}

// ============================================ string END ============================================

// ====================================== ll_couple_vector START ========================================

vect_ll_couple_t vect_ll_couple_const()
{
    T_vect_ll_couple *tmp = calloc(1, sizeof(T_vect_ll_couple) + STRING_START_SIZE * sizeof(token_couple_t));
    if (tmp == NULL)
    {
        exit(MEM_ERR);
    }
    tmp->pos = VECTOR_START_POS;
    tmp->size = VECTOR_START_SIZE;
    MMU_add(tmp);

    return tmp;
}

void vllc_push_fisrt(vect_ll_couple_t *vector, token_t value)
{
    T_vect_ll_couple *tmp = *vector;
    if (++tmp->pos == tmp->size)
    {
        tmp->size *= 2;
        *vector = tmp = MMU_realloc_item(tmp, tmp->size * sizeof(token_couple_t) + sizeof(T_vect_ll_couple));
    }
    tmp->values[tmp->pos][0] = value;
}

void vllc_add_second(vect_ll_couple_t vector, token_t value)
{
    vector->values[vector->pos][1] = value;
}

void vllc_pop_couple(vect_ll_couple_t vector, token_t *first, token_t *second)
{
    *first = vector->values[vector->pos][0];
    *second = vector->values[vector->pos--][1];
}

token_t vllc_get_last_second(vect_ll_couple_t vector)
{
    return vector->values[vector->pos][1];
}

int64_t vllc_pos(vect_ll_couple_t vector)
{
    return vector->pos;
}

void discard_ll_couple_vector(vect_ll_couple_t *vector)
{
    MMU_discard(*vector);
    *vector = NULL;
}

// ======================================= ll_couple_vector END =========================================

// ======================================== token_vector START ==========================================

vect_token_t vect_token_const()
{
    T_vect_token *tmp = calloc(1, sizeof(T_vect_token) + STRING_START_SIZE * sizeof(token_t));
    if (tmp == NULL)
    {
        exit(MEM_ERR);
    }
    tmp->pos = VECTOR_START_POS;
    tmp->size = VECTOR_START_SIZE;
    MMU_add(tmp);

    return tmp;
}

void vtn_push(vect_token_t *vector, token_t value)
{
    T_vect_token *tmp = *vector;
    if (++tmp->pos == tmp->size)
    {
        tmp->size *= 2;
        *vector = tmp = MMU_realloc_item(tmp, tmp->size * sizeof(token_t) + sizeof(T_vect_token));
    }
    tmp->values[tmp->pos] = value;
}

token_t vtn_get_2nd_last(vect_token_t vector)
{
    return vector->values[vector->pos -1];
}

token_t vtn_get_last(vect_token_t vector)
{
    return vector->values[vector->pos];
}

void vtn_replace_last_2(vect_token_t vector, token_t token)
{
    vector->pos--;
    vector->values[vector->pos] = token;
}

token_t vtn_pop(vect_token_t vector)
{
    return vector->values[vector->pos--];
}

void vtn_reset(vect_token_t vector)
{
    vector->pos = VECTOR_START_POS;
}

void discard_token_vector(vect_token_t *vector)
{
    MMU_discard(*vector);
    *vector = NULL;
}

void discard_token_vector_free(vect_token_t *vector)
{
    if (*vector != NULL)
    {
        vect_token_t tmp = *vector;
        for (int64_t i = 0; i <= tmp->pos; i++)
        {
            free(tmp->values[i]);
        }
        MMU_discard(tmp);
        *vector = NULL;
    }
}

// =========================================== ll_vector END ===========================================

// ====================================== double_vect_func START =======================================

vect_dbl_func_t vect_dbl_func_const()
{
    T_dvect_func *tmp = calloc(1, sizeof(T_dvect_func) + VECTOR_START_SIZE * sizeof(htab_func_t));
    if (tmp == NULL)
        exit(MEM_ERR);
    
    tmp->pos = VECTOR_START_POS;
    tmp->size = VECTOR_START_SIZE;
    tmp->reader = VECTOR_START_POS;

    MMU_add(tmp);
    return (vect_dbl_func_t)tmp;
}

void vdfht_push(vect_dbl_func_t *vector, vect_htab_func_t value)
{
    vect_dbl_func_t tmp = *vector;
    if (++tmp->pos == tmp->size)
    {
        tmp->size *= 2;
        *vector = tmp = MMU_realloc_item(tmp, tmp->size * sizeof(htab_func_t) + sizeof(T_vect_func));
    }
    tmp->values[tmp->pos] = value;
}

vect_htab_func_t vdfht_get_next(vect_dbl_func_t vector)
{
    if (++vector->reader > vector->pos)
    {
        return NULL;
    }

    return vector->values[vector->reader];
}

void reset_dbl_func_vector(vect_dbl_func_t vector)
{
    vector->pos = VECTOR_START_POS;
    vector->reader = VECTOR_START_POS;
}

void discard_dbl_func_vector(vect_dbl_func_t *vector)
{
    MMU_discard(*vector);
    *vector = NULL;
}

// ======================================= double_vect_func END ========================================

// ======================================== vect_var_val START =========================================

vect_var_val_t vect_var_val_const()
{
    T_vect_vval *tmp = calloc(1, sizeof(var_values_t)*VECTOR_START_SIZE + sizeof(T_vect_vval));
    if (tmp == NULL)
        exit(MEM_ERR);
    
    tmp->pos = VECTOR_START_POS;
    tmp->size = VECTOR_START_SIZE;

    MMU_add(tmp);

    return (vect_var_val_t)tmp;
}

void vvv_push(vect_var_val_t *vector, var_values_t value)
{
    vect_var_val_t tmp = *vector;
    if (++tmp->pos == tmp->size)
    {
        tmp->size *= 2;
        *vector = tmp = MMU_realloc_item(tmp, tmp->size * sizeof(var_values_t) + sizeof(T_vect_vval));
    }
    tmp->values[tmp->pos] = value;
}

void vvv_push_copy(vect_var_val_t *vector)
{
    vect_var_val_t tmp = *vector;
    if (++tmp->pos == tmp->size)
    {
        tmp->size *= 2;
        *vector = tmp = MMU_realloc_item(tmp, tmp->size * sizeof(var_values_t) + sizeof(T_vect_vval));
    }
    tmp->values[tmp->pos] = tmp->values[tmp->pos - 1];
}

void vvv_change_last_copy(vect_var_val_t vector)
{
    vector->values[vector->pos] = vector->values[vector->pos - 1];
}

var_values_t vvv_get_last(vect_var_val_t vector)
{
    return vector->values[vector->pos];
}

var_values_t vvv_get_offset(vect_var_val_t vector, int64_t offset)
{
    return vector->values[vector->pos - offset];
}

bool vvv_compare_last_two(vect_var_val_t vector, uint8_t type)
{
    switch(type)
    {
        case T_STRING:
            return !strcmp(vector->values[vector->pos].string, vector->values[vector->pos - 1].string);
        
        case T_INT:
            return vector->values[vector->pos].integer == vector->values[vector->pos - 1].integer;
        
        case T_FLOAT:
            return vector->values[vector->pos].float64 == vector->values[vector->pos - 1].float64;
        
        case T_BOOL:
            return vector->values[vector->pos].boolean == vector->values[vector->pos - 1].boolean;
    }
    return false;
}

void vvv_change_last(vect_var_val_t vector, var_values_t value)
{
    vector->values[vector->pos] = value;
}

var_values_t vvv_pop(vect_var_val_t vector)
{
    return vector->values[vector->pos--];
}

void vvv_free_last_string(vect_var_val_t vector)
{
    if (vector->pos == 0)
    {
        free(vector->values[0].string);
    }
    else if (vector->pos > 0 && strcmp(vector->values[vector->pos].string, vector->values[vector->pos - 1].string))
    {
        free(vector->values[vector->pos].string);
    }
}

void vvv_free_reset_strings(vect_var_val_t vector)
{
    while (vector->pos >= 0)
    {
        if (vector->pos == 0)
        {
            free(vector->values[0].string);
        }
        else if (vector->pos > 0 && strcmp(vector->values[vector->pos].string, vector->values[vector->pos - 1].string))
        {
            free(vector->values[vector->pos].string);
        }
        vector->pos--;
    }
}

int64_t vvv_pos(vect_var_val_t vector)
{
    return vector->pos;
}

void vect_var_val_reset(vect_var_val_t vector)
{
    vector->pos = VECTOR_START_POS;
}

void discard_var_val_vector(vect_var_val_t *vector)
{
    MMU_discard(*vector);
    *vector = NULL;
}

// ========================================= vect_var_val END ==========================================
