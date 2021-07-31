
//=========================================================================================================
// File:        memory.c
// Case:        VUT, FIT, IFJ, compiler project
// Date:        9. 12. 2020
// Author(s):   David Mihola
// Contac(s):   xmihol00@stud.fit.vutbr.cz
// Compiled:    gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)
// Description: File containing definitions of functions representing the memory management unit (MMU).
//==========================================================================================================

#include "memory_private.h"

/**
 * @struct The memory managament unit for all pointers which are not global and cannot be deallocated on a program exit
**/
struct
{
    uint64_t size;    /// the current size of the allocated memory for the memory pointers of the memory managament unit
    uint64_t load;    /// the number of a currently stored pointers, if load/size > 0.6 rehashing is performed
    void **poiners;   /// dynamically allocated array of pointers to the memory
}G_MMU = {0, };

void init_MMU()
{
    //if it is already constructed
    if (G_MMU.size != 0)
        return;

    G_MMU.poiners = calloc(MMU_START_SIZE, sizeof(void *));
    if (G_MMU.poiners == NULL)
        exit(MEM_ERR);
    
    G_MMU.size = MMU_START_SIZE;
    G_MMU.load = MMU_START_LOAD;
}

void dispose_MMU()
{
    // deallocation of all the memory
    for (uint64_t i = 0; i < G_MMU.size; i++)
    {
        free(G_MMU.poiners[i]);
    }

    // deallocation of the MMU
    free(G_MMU.poiners);
    G_MMU.size = 0;
    G_MMU.load = MMU_START_LOAD;
}

void MMU_add(void *ptr)
{
    if ((double)++G_MMU.load / (double)G_MMU.size > MMU_REHASH_VAL)
    {
        // if the load factor exceedes 0.625
        MMU_rehash();
    }

    //the pointer values are always multiples of 8, only when dividing by 32 the modulated numbers starts to differ
    uint64_t i = (((uint64_t)ptr) >> 5) % G_MMU.size; //hash function

    // there must always be some NULL position, as the load factor is 0.625
    while (G_MMU.poiners[i % G_MMU.size] != NULL)
    {
        i++;
    }
    
    G_MMU.poiners[i % G_MMU.size] = ptr;
}

void MMU_discard(void *ptr)
{
    if (ptr == NULL)
    {
        return;
    }
    //the pointer values are always multiples of 8, only when dividing by 32 the modulated numbers starts to differ
    uint64_t i = (((uint64_t)ptr) >> 5) % G_MMU.size;
    
    for (uint64_t j = 0; G_MMU.poiners[i % G_MMU.size] != ptr; j++)
    {
        // should never happen
        if (j == G_MMU.size)
        {
            exit(INTERNAL_ERR);
        }
        i++;
    }
    
    free(G_MMU.poiners[(i % G_MMU.size)]);
    G_MMU.poiners[(i % G_MMU.size)] = NULL;

    G_MMU.load--;
}

void MMU_rehash()
{
    uint64_t new_size = G_MMU.size * 2;
    
    // create new table with doubled size
    void **tmp = calloc(new_size, sizeof(void *));
    if (tmp == NULL)
    {
        exit(MEM_ERR);
    }

    uint64_t j;

    // rehash the table
    for (uint64_t i = 0; i < G_MMU.size; i++)
    {
        if (G_MMU.poiners[i] != NULL)
        {
            j = i;
            while (tmp[j % new_size] != NULL)
                j++;
            
            tmp[j % new_size] = G_MMU.poiners[i];
        }
    }

    // deallocate the old one
    free(G_MMU.poiners);
    G_MMU.poiners = tmp;
    G_MMU.size = new_size;
}

void *MMU_realloc_item(void *ptr, uint64_t new_size)
{
    //the pointer values are always multiples of 8, only when dividing by 32 the modulated numbers starts to differ
    uint64_t i = (((uint64_t)ptr) >> 5) % G_MMU.size;

    // find the item
    for (uint64_t j = 0; G_MMU.poiners[i % G_MMU.size] != ptr; j++)
    {
        // should never happen
        if (j == G_MMU.size)
        {
            exit(INTERNAL_ERR);
        }
        i++;
    }

    // realocate its size
    void *tmp = realloc(G_MMU.poiners[i % G_MMU.size], new_size);
    if (tmp == NULL)
    {
        exit(MEM_ERR);
    }

    // store the reallocated size
    G_MMU.poiners[i % G_MMU.size] = tmp;
    
    return tmp;
}
