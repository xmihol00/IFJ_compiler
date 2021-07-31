
#ifndef __TOKEN_PRIVATE_H__
#define __TOKEN_PRIVATE_H__

//=========================================================================================================
// File:        token_private.h
// Case:        VUT, FIT, IFJ, compiler project
// Date:        9. 12. 2020
// Author(s):   David Mihola, Vladislav Sokolovskii
// Contac(s):   xmihol00@stud.fit.vutbr.cz, sokol15@stud.fit.vutbr.cz
// Compiled:    gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)
// Description: Header file containing private data structure declaration for the token module.
//==========================================================================================================

#include "token.h"


struct token {
    uint8_t type;       ///type of the token
    token_data_t data;  ///data of the token, if there is some
    struct token *next; ///pointer to the next token in the list
};


#endif //__TOKEN_PRIVATE_H__
