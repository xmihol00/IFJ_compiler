
//=========================================================================================================
// File:        functions.c
// Case:        VUT, FIT, IFJ, compiler project
// Date:        9. 12. 2020
// Author(s):   Lukas Foltyn
// Contac(s):   xfolty17@stud.fit.vutbr.cz
// Compiled:    gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)
// Description: Definitions of functions generating target IFJ20code instructions of built-in functions.
//==========================================================================================================

#include "functions.h"

static const char *get_type(uint8_t type)
{
	switch(type)
	{
		case(T_STRING):
			return "string";
		case(T_INT):
			return "int";
		case(T_FLOAT):
			return "float";
		case(T_BOOL):
			return "bool";
		default:
			break;	
	}
	return NULL;
}	

char *get_func_data_type(uint8_t func_type)
{
	switch(func_type)
	{
		case(T_INS):
			return "string";
		case(T_INI):
			return "int";
		case(T_INF):
			return "float";
		case(T_INB):
			return "bool";
		default:
			break;	
	}
	return NULL;
}	

void c_input(uint8_t type)
{
	static unsigned label_counter = 1;

	printf("READ GF@?temp&1 %s\n", get_type(type)); //read to temporary variable	
	printf("PUSHS GF@?temp&1\n"); //push it on stack
	printf("TYPE GF@?temp&2 GF@?temp&1\n"); //check the type 
	printf("JUMPIFEQ input?sucess&%u GF@?temp&2 string@%s\n", label_counter, get_type(type)); // error - success ?
	printf("PUSHS int@1\n"); //push 1 as error if types don't match
	printf("JUMP input?error&%u\n", label_counter);
	printf("LABEL input?sucess&%u\n", label_counter);
	printf("PUSHS int@0\n"); //push 0 as sucess if types match
	printf("LABEL input?error&%u\n", label_counter);

	label_counter++;
}


void c_print()
{
	printf("POPS GF@?temp&1\n");
	printf("WRITE GF@?temp&1\n");
}



void c_int2float()
{
	printf("INT2FLOATS\n");

}
void c_float2int()
{
	printf("FLOAT2INTS\n");
}
void c_len()
{
	printf("POPS GF@?temp&1\n");
	printf("STRLEN GF@?temp&2 GF@?temp&1\n");
	printf("PUSHS GF@?temp&2\n");		
}

void c_substr()
{
	static unsigned label_counter = 1;

	printf("STRLEN GF@?temp&4 GF@?temp&3\n"); //stlren of string
	printf("LT GF@?temp&5 GF@?temp&2 GF@?temp&4\n"); 
	printf("JUMPIFNEQ substr?error%u GF@?temp&5 bool@true\n",label_counter); // --> jump to error if i > maxindex
	printf("LT GF@?temp&5 GF@?temp&1 int@0\n"); 
	printf("JUMPIFEQ substr?error%u GF@?temp&5 bool@true\n",label_counter); // --> jump to error if i < 0
	printf("LT GF@?temp&5 GF@?temp&2 int@0\n"); 
	printf("JUMPIFEQ substr?error%u GF@?temp&5 bool@true\n",label_counter); // --> jump to error if n < 0
	
	printf("MOVE GF@?temp&5 string@\n"); //creating new empty string
	
	printf("LABEL substr?forloop%u\n",label_counter); // for loop start
	printf("JUMPIFEQ substr?endloop%u GF@?temp&1 int@0\n",label_counter); // end loop when n bytes were copied
	printf("JUMPIFEQ substr?endloop%u GF@?temp&2 GF@?temp&4\n",label_counter); // end loop if index reaches one behind the end of given string
	printf("GETCHAR GF@?temp&6 GF@?temp&3 GF@?temp&2\n"); //getting another character as string
	printf("CONCAT GF@?temp&5 GF@?temp&5 GF@?temp&6\n"); //concat
	printf("ADD GF@?temp&2 GF@?temp&2 int@1\n"); //increment index
	printf("SUB GF@?temp&1 GF@?temp&1 int@1\n"); //decrement n - bytes to copy
	printf("JUMP substr?forloop%u\n",label_counter);
	printf("LABEL substr?endloop%u\n",label_counter);
	
	printf("PUSHS GF@?temp&5\n"); //push created string
	printf("PUSHS int@0\n"); //push 0 that indicates success of function			
	printf("JUMP substr?success%u\n",label_counter);
	printf("LABEL substr?error%u\n",label_counter);
	printf("PUSHS nil@nil\n"); //push null string as error?
	printf("PUSHS int@1\n"); //push 1 that indicates error in function
	printf("LABEL substr?success%u\n",label_counter);

	label_counter++;
}
void c_ord()
{
	static unsigned label_counter = 1;

	printf("STRLEN GF@?temp&3 GF@?temp&2\n"); //strlen of string
	printf("LT GF@?temp&3 GF@?temp&1 GF@?temp&3\n"); //checking if index is not out of range
	printf("JUMPIFNEQ ord?error%u GF@?temp&3 bool@true\n",label_counter);
	printf("LT GF@?temp&3 GF@?temp&1 int@0\n");
	printf("JUMPIFEQ ord?error%u GF@?temp&3 bool@true\n",label_counter); // checking index > 0
	printf("STRI2INT GF@?temp&3 GF@?temp&2 GF@?temp&1\n");
	printf("PUSHS GF@?temp&3\n");
	printf("PUSHS int@0\n"); //sucess
	printf("JUMP ord?success%u\n",label_counter);		
	printf("LABEL ord?error%u\n",label_counter);
	printf("PUSHS nil@nil\n"); //error
	printf("PUSHS int@1\n");
	printf("LABEL ord?success%u\n",label_counter);

	label_counter++;
}
void c_chr()
{
	static unsigned label_counter = 1;
	
	printf("LT GF@?temp&2 GF@?temp&1 int@0\n"); //check the value of char > 0
	printf("JUMPIFEQ chr?error%u GF@?temp&2 bool@true\n",label_counter);
	printf("GT GF@?temp&2 GF@?temp&1 int@255\n");
	printf("JUMPIFEQ chr?error%u GF@?temp&2 bool@true\n",label_counter); //check the value of char < 255
	printf("INT2CHAR GF@?temp&2 GF@?temp&1\n");
	printf("PUSHS GF@?temp&2\n"); //sucess
	printf("PUSHS int@0\n"); 
	printf("JUMP chr?success%u\n",label_counter);		
	printf("LABEL chr?error%u\n",label_counter);
	printf("PUSHS nil@nil\n"); //error
	printf("PUSHS int@1\n");
	printf("LABEL chr?success%u\n",label_counter);		

	label_counter++;	
}
