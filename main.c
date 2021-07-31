
//=========================================================================================================
// File:        main.c
// Case:        VUT, FIT, IFJ, compiler project
// Date:        9. 12. 2020
// Author(s):   David Mihola, Lukas Foltyn, Vladislav Sokolovskii
// Contac(s):   xmihol00@stud.fit.vutbr.cz, xfolty17@stud.fit.vutbr.cz, xsokol15@stud.fit.vutbr.cz
// Compiled:    gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)
// Description: File containing the main function of the programm. Other modules are called from the main
//              function, which are responsible for different parts of the execution.
//==========================================================================================================

#include "main.h"

int main()
{
    atexit(dispose_program); // correct deallocation of all allocated memory

    init_program();          // initialization of all data structures (all modules)

    syntax_analysis();       // the syntax analysis of the source, uses scanner - lexical analysis
    
    print_list();

    semantic_analysis();     // the semantic analysis of the source code
                             // (could be done simultaneously with generation, but optimization requires semantically correct program)

    optimizer();             //optimization of the compiler intermediate code

    generator();             //generation of the targeted IFJcode20 program

    return 0;                // successfully compiled program
}

void init_program()
{
    init_MMU();
    init_global_ht();
    init_func_ht();
    init_parser();
    init_orpn();
    init_list();
}

void dispose_program()
{
    dispose_orpn();
    dispose_list();
    dispose_global_ht();
    dispose_func_ht();
    dispose_parser();
    dispose_sematic();
    dispose_optimizer();
    dispose_generator();
    dispose_register();
    dispose_MMU();
}
