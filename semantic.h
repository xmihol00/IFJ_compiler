
#ifndef __SEMANTIC_H__
#define __SEMANTIC_H__

//=========================================================================================================
// File:        semantic.h
// Case:        VUT, FIT, IFJ, compiler project
// Date:        9. 12. 2020
// Author(s):   David Mihola
// Contac(s):   xmihol00@stud.fit.vutbr.cz
// Compiled:    gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)
// Description: Header file containing declarations of functions accessible for other modules.
//==========================================================================================================

#include "symtable.h"
#include "enums.h"
#include "vector.h"
#include "functions.h"

void init_semantic();

void dispose_sematic();

void semantic_analysis();

#endif //__SEMANTIC_H__
