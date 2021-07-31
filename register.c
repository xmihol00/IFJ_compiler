
//=========================================================================================================
// File:        register.c
// Case:        VUT, FIT, IFJ, compiler project
// Date:        9. 12. 2020
// Author(s):   David Mihola
// Contac(s):   xmihol00@stud.fit.vutbr.cz
// Compiled:    gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)
// Description: File containing defintions of functions responsible for manipulation with registers.
//==========================================================================================================

#include "register.h"

struct 
{
    bool indirect;
    int32_t size;
    int32_t pos;
    int32_t current;
    reg_t *regs;
}G_register = {0,};

void init_register()
{
    G_register.regs = calloc(REG_START_SIZE, sizeof(reg_t));
    if (G_register.regs == NULL)
    {
        exit(MEM_ERR);
    }

    sprintf(G_register.regs[0], "GF@r%%1&"); // creates the first register immidiately
    G_register.size = REG_START_SIZE;
    G_register.pos = REG_START_POS + 1;
    G_register.current = REG_START_POS;
}

void dispose_register()
{
    free(G_register.regs);
}

char *reg_get()
{
    if (++G_register.current > G_register.pos) // count of defined registers is not enought
    {
        if (++G_register.pos == G_register.size) // size of the dynamic array is to small
        {
            G_register.size *= 2;
            void *tmp = realloc(G_register.regs, G_register.size * sizeof(reg_t));
            if (tmp == NULL)
            {
                exit(MEM_ERR);
            }

            G_register.regs = (reg_t *)tmp;
        }

        // new register is created
        sprintf(G_register.regs[G_register.pos], "GF@r%%%d&", G_register.pos + 1);
    }

    return G_register.regs[G_register.current];
}

char *reg_get_indirect()
{
    G_register.indirect = true;
    return G_register.regs[++G_register.current];
}

void reg_reset()
{
    G_register.indirect = false;
    G_register.current = REG_START_POS; // resets the position, values are kept
}

char *reg_get_last()
{
    return G_register.regs[G_register.current]; 
}

void reg_set_emtpy()
{
    if (!G_register.indirect || G_register.current > 0)
    {
        G_register.current--; // register is emptied
    }
}

void define_used_registers(string_t *prog_header)
{
    string_substr_push(prog_header, "\n# registers definition\n", 24);
    for (int32_t i = 0; i <= G_register.pos; i++)
    {   
        // loads the used registers to the program header, then printed by the generator
        string_substr_push(prog_header, "DEFVAR ", 7);
        string_substr_push(prog_header, G_register.regs[i], strlen(G_register.regs[i]));
        string_char_push(prog_header, '\n');
    }
}
