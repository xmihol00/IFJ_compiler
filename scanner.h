
#ifndef __SCANNER_H__
#define __SCANNER_H__

//=========================================================================================================
// File:        scanner.h
// Case:        VUT, FIT, IFJ, compiler project
// Date:        9. 12. 2020
// Author(s):   Lukas Foltyn, Vladislav Sokolovskii
// Contac(s):   xfolty17@stud.fit.vutbr.cz, xsokol15@stud.fit.vutbr.cz
// Compiled:    gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)
// Description: Header file containing constant and function declarations accessible for other modules.
//==========================================================================================================

#include "vector.h"
#include "enums.h"

#define NOT_KEYWORD 0
#define NOT_BUILD_IN 0

int scanner(string_t *output_data);

#endif // __SCANNER_H__
