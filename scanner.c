
//=========================================================================================================
// File:        scanner.c
// Case:        VUT, FIT, IFJ, compiler project
// Date:        9. 12. 2020
// Author(s):   Lukas Foltyn, Vladislav Sokolovskii, David Mihola - bug fixes
// Contac(s):   xfolty17@stud.fit.vutbr.cz, xsokol15@stud.fit.vutbr.cz
// Compiled:    gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)
// Description: File containing function definitions responsible for the lexical analysis of the source code
//==========================================================================================================

#include "scanner_private.h"

int scanner(string_t *output_data)
{
    int c;
    while (true)
    {
        c = getchar();
        if (isalpha(c) || c == '_')
        {
            //start of an identifier or keyword, calls partial automat to process this task
            return keyword_idf(output_data, c);
        }
        else if (isdigit(c))
        {
            //start of a number, calls partial automat to process this task
            number(output_data, c);
            return T_NUM;
        }   
        switch(c) // other characters
        {
            case EOF:
                return EOF;
            
            case ' ':
                continue; //search for token continues
            
            case '\t':
                continue; //search for token continues
            
            case '\n':
                return T_NL;

            case '(': 
                return T_OB;

            case ')':
                return T_CB;
            
            case '{':
                return T_OCB;
            
            case '}':
                return T_CCB;

            case ',':
                return T_COMMA;
            
            case ';':
                return T_SEMI;
            // all above have unique values, so the parser won't need output data,
            // as the return token will match the output data
            
            case '+':
                c = getchar();
                if(c == '=')
                {
                    string_char_push(output_data, (char)ADD_EQ);
                    return T_EQ;
                }
                else
                {
                    ungetc(c, stdin);
                    string_char_push(output_data, ADD); 
                }
                return T_OP;
            
            case '-':
                c = getchar();
                if(c == '=')
                {
                    string_char_push(output_data, (char)SUB_EQ);
                    return T_EQ;
                }
                else
                {
                    ungetc(c, stdin);
                    string_char_push(output_data, SUB); 
                }
                return T_OP;
            
            case '*':
                c = getchar();
                if(c == '=')
                {
                    string_char_push(output_data, (char)MUL_EQ);
                    return T_EQ;
                }
                else
                {
                    ungetc(c, stdin);
                    string_char_push(output_data, MUL); 
                }
                return T_OP;
            
            case '/':
                c = getchar();
                if(c == '=')
                {
                    string_char_push(output_data, (char)DIV_EQ);
                    return T_EQ;
                }
                else if (c == '/' || c == '*')
                {
                    ungetc(c, stdin);
                    skip_comment();
                    break; //search for token continues
                }
                else
                {
                    ungetc(c, stdin);
                    string_char_push(output_data, DIV); 
                }
                return T_OP;
            
            case '<':
                c = getchar();
                if(c == '=')
                {
                    string_char_push(output_data, LTE);
                }
                else
                {
                    ungetc(c, stdin);
                    string_char_push(output_data, LT);
                }
                return T_OP;
            
            case '>':
                c = getchar();
                if(c == '=')
                {
                    string_char_push(output_data, GTE);
                }
                else
                {
                    ungetc(c, stdin);
                    string_char_push(output_data, GT);
                }           
                return T_OP;
            
            case '!':
                c = getchar();
                if(c == '=')
                {
                    string_char_push(output_data, NEQ);
                    return T_OP;
                }
                else
                {
                    ungetc(c, stdin);
                    string_char_push(output_data, NOT);
                    return T_OP;
                }
            
            case '=':
                c = getchar();
                if (c == '=')
                {
                    string_char_push(output_data, EQ);
                    return T_OP;
                }
                else
                {
                    ungetc(c, stdin);
                    string_char_push(output_data, (char)EQ_ASSGN);
                    return T_EQ; 
                }
            
            case ':':
                c = getchar();
                if (c == '=')
                {
                    string_char_push(output_data, (char)EQ_DEF);
                    return T_EQ;
                }
                exit(LEXICAL_ERR);
            
            case '"':
                convert_string(output_data);
                return T_STRING;
            
            case '&':
                c = getchar();
                if (c == '&')
                {
                    string_char_push(output_data, (char)AND);
                    return T_OP;
                }
                else
                    exit(LEXICAL_ERR);
                
            case '|':
                c = getchar();
                if (c == '|')
                {
                    string_char_push(output_data, (char)OR);
                    return T_OP;
                }
                else
                    exit(LEXICAL_ERR);
            
            default:
                exit(LEXICAL_ERR); //doesn't match any of allowed characters
                break;
        }
    }

}

void convert_string(string_t *output_data)
{
        /*
        Converts a string literal, which first character (the character '"') is at G_reader.program[G_reader.head], 
        to an ASCII string and saves it to G_parser.lexem.string. For full detail see the assignment
    */
    char tmp[16] = "";
    int curr_char = getchar();
    bool coded_bslash = false;

    while (true) {
        if (curr_char == '\n' || curr_char == EOF)
        {
            exit(LEXICAL_ERR);
        }
        else if (curr_char <= 32 || curr_char == 35 || (curr_char == 92 && coded_bslash) || isspace(curr_char) || curr_char > 126) 
        {
            sprintf(tmp, "\\%03u", curr_char);
            string_substr_push(output_data, tmp, 4);
        }
        else if (curr_char > 32 && curr_char != '"' && curr_char != '\\') {
            string_char_push(output_data, (char)curr_char);
        }
        else if (curr_char == '\\') {
            curr_char = retrieve_coded_symbol();
            coded_bslash = true;

            if (curr_char <= 32 || curr_char == 35 || curr_char == 92 || isspace(curr_char) || curr_char > 126) 
            {
                sprintf(tmp, "\\%03u", curr_char);
                string_substr_push(output_data, tmp, 4);
            }
            else
            {
                string_char_push(output_data, curr_char);
            }
        }
        else if (curr_char == '"') { //the end of the string literal
            return;
        }

        curr_char = getchar();
        coded_bslash = false;
    }
}

int retrieve_coded_symbol()
{
    int curr_char = getchar();

    switch (curr_char) {
        case '"' :
            return '"';
        case 'n' :
            return '\n';
        case '\\' :
            return '\\';
        case 't' :
            return '\t';
        case 'x' :
            return hex_to_ascii();
        default:
            exit(LEXICAL_ERR);
    }
}

int hex_to_ascii() 
{
    int val;
    int decimal = 0;
    int curr_char;

    for (int i = 0; i < 2; ++i) {
        curr_char = getchar();
        if (curr_char >= '0' && curr_char <= '9') {
            val = (int)curr_char - 48;
        }
        else if (curr_char >= 'A' && curr_char <= 'F') {
            val = (int)curr_char - 65 + 10;
        }
        else if (curr_char >= 'a' && curr_char <= 'f') {
            val = (int)curr_char - 97 + 10;
        }
        else
            exit(LEXICAL_ERR);

        decimal += val * (i == 0 ? 16 : 1); // val1 * 16^1 + val2 * 16^0
    }

    return decimal; 
}

void skip_comment()
{
    /*
        Skips a program comment, which first character '/' is at G_reader.program[G_reader.head], if the comment is
        multi-line calls function reader_read_line() in the process
    */
    int curr_char = getchar();
    int last_char = 0;

    if (curr_char == '/') {
        //one line comment detected, G_reader.head will be moved to the end of the line
        //according to the task specification new line character is not considered as a part of comment
        while ((curr_char = getchar()) != '\n' && curr_char != EOF);
        
        if (curr_char == EOF)
        {   
            ungetc(curr_char, stdin);
            return;
        }
        else
        {
            ungetc(curr_char, stdin); 
        }
        return;
    }
    else if (curr_char == '*') 
    {
        //multi-line comment
        while (true) 
        {
            while (curr_char = getchar(), curr_char != EOF)
            {

                if (last_char  == '*' && curr_char == '/') {
                    return;
                }
                last_char = curr_char;
            }
            if (curr_char == EOF)
            {
                exit(LEXICAL_ERR);
            }
        }
    }
}

void number(string_t *output_data, int curr_ch)
{
    enum states {
        coded_num,
        hex_num,
        oct_num,
        bin_num,
        integer,
        float64,//state when exponent was already detected
        float64dot,//state when current char is a point
        float64fract,//state when current char is in the fraction part (no exponent was detected yet)
        float64exp,//state when current char is the exponent char 'e' || 'E'
        float64exp_sign,//state when the current char is a sign after the exponent sign
    };
    int state; //current state
    bool underscore = false; //set to true when underscore was read
    bool only_number_expected = false;
    state = integer;
    bool first_zero = false; // set to true if the first character of number is zero
    if (curr_ch == '0') {
        first_zero = true;
    }
    string_char_push(output_data, curr_ch);
    while (1) {
        curr_ch = getchar();

        switch (state) {
            case coded_num: {
                if (curr_ch == 'x' || curr_ch == 'X') {
                    if (underscore){
                        exit(LEXICAL_ERR);
                    }
                    string_char_push(output_data, (char)curr_ch);
                    only_number_expected = true;
                    state = hex_num;
                }
                else if (curr_ch == 'o' || curr_ch == 'O' || isdigit(curr_ch)) {//octal prefix starts with 0, o and 0 are omitted
                    if (underscore){
                        exit(LEXICAL_ERR);
                    }
                    string_char_push(output_data, (char)curr_ch);
                    only_number_expected = true;
                    state = oct_num;
                }
                else if (curr_ch == 'b' || curr_ch == 'B') {
                    if (underscore){
                        exit(LEXICAL_ERR);
                    }
                    string_char_push(output_data, (char)curr_ch);
                    only_number_expected = true;
                    state = bin_num;
                }
                else if (curr_ch == '_') {
                    if (underscore){
                        exit(LEXICAL_ERR); //'_' must separate successive digits
                    }
                    underscore = true; //last read char was underscore
                }
                else {
                    exit(LEXICAL_ERR);
                }
            }
            break;

            case integer: {
                if (isdigit(curr_ch)) {
                    if (first_zero) { // octal number starts with zero
                        state = oct_num;
                        ungetc(curr_ch, stdin);
                        continue;
                    }
                    string_char_push(output_data, (char)curr_ch);
                    first_zero = false; //the next zero will not be the first in the string
                }
                else if (curr_ch == '_') {
                    if (underscore){
                        exit(LEXICAL_ERR); //'_' must separate successive digits
                    }
                    underscore = true; //last read char was underscore
                }
                else if (curr_ch == '.') {
                    if (underscore){
                        exit(LEXICAL_ERR);
                    }
                    string_char_push(output_data, (char)curr_ch);
                    state = float64dot;
                }
                else if (curr_ch == 'e' || curr_ch == 'E') {
                    if (underscore) {
                        exit(LEXICAL_ERR);
                    }
                    string_char_push(output_data, (char)curr_ch);
                    state = float64exp;
                }
                else if (isalpha(curr_ch) && first_zero) {
                    ungetc(curr_ch, stdin);
                    state = coded_num;
                }
                else {
                    if (underscore) {
                        exit(LEXICAL_ERR);
                    }
                    ungetc(curr_ch, stdin);
                    string_char_push(output_data, (char) T_INT);//insert type to the end of the output string
                    return;
                }
            }
            break;

            case hex_num: {
                if (isdigit(curr_ch) || (curr_ch >= 'a' && curr_ch <= 'f') || (curr_ch >= 'A' && curr_ch <= 'F')) {
                    string_char_push(output_data, (char)curr_ch);
                }
                else if (curr_ch == '_') {
                    if (underscore){
                        exit(LEXICAL_ERR); //'_' must separate successive digits
                    }
                    underscore = true; //last read char was underscore
                }
                else if (only_number_expected && !isdigit(curr_ch)) {
                    exit(LEXICAL_ERR);
                }
                else {
                    if (underscore) {
                        exit(LEXICAL_ERR);
                    }
                    ungetc(curr_ch, stdin);
                    string_char_push(output_data, (char) T_INT);//insert type to the end of the output string
                    return;
                }
                only_number_expected = false;
            }
            break;

            case oct_num: {
                if (curr_ch >= '0' && curr_ch <= '7') {
                    string_char_push(output_data, (char)curr_ch);
                }
                else if (curr_ch == '_') {
                    if (underscore){
                        exit(LEXICAL_ERR); //'_' must separate successive digits
                    }
                    underscore = true; //last read char was underscore
                }
                else if (only_number_expected && !isdigit(curr_ch)) {
                    exit(LEXICAL_ERR);
                }
                else {
                    if (underscore || isdigit(curr_ch)) {
                        exit(LEXICAL_ERR);
                    }
                    ungetc(curr_ch, stdin);
                    string_char_push(output_data, (char) T_INT);//insert type to the end of the output string
                    return;
                }
                only_number_expected = false;
            }
            break;

            case bin_num : {
                if (curr_ch == '0' || curr_ch == '1') {
                    string_char_push(output_data, (char)curr_ch);
                }
                else if (curr_ch == '_') {
                    if (underscore){
                        exit(LEXICAL_ERR); //'_' must separate successive digits
                    }
                    underscore = true; //last read char was underscore
                }
                else if (only_number_expected && !isdigit(curr_ch)) {
                    exit(LEXICAL_ERR);
                }
                else {
                    if (underscore || isdigit(curr_ch)) {
                        exit(LEXICAL_ERR);
                    }
                    ungetc(curr_ch, stdin);
                    string_char_push(output_data, (char) T_INT);//insert type to the end of the output string
                    return;
                }
                only_number_expected = false;
            }
                break;
            case float64dot : {
                if (isdigit(curr_ch)) {
                    string_char_push(output_data, (char)curr_ch);
                    state = float64fract;
                }
                else if (curr_ch == '_') {
                    exit(LEXICAL_ERR); //'_' must separate successive digits
                }
                else {
                    ungetc(curr_ch, stdin);
                    exit(LEXICAL_ERR);
                }
            }
            break;

            case float64fract : {
                if (isdigit(curr_ch)) {
                    string_char_push(output_data, (char)curr_ch);
                }
                else if (curr_ch == '_'){
                    if (underscore){
                        exit(LEXICAL_ERR); //'_' must separate successive digits
                    }
                    underscore = true; //last read char was underscore
                }
                else if (curr_ch == 'e' || curr_ch == 'E') {
                    if (underscore) {
                        exit(LEXICAL_ERR);
                    }
                    string_char_push(output_data, (char)curr_ch);
                    state = float64exp;
                }
                else if(curr_ch == '.'){
                    exit(LEXICAL_ERR);
                }
                else {
                    if (underscore) {
                        exit(LEXICAL_ERR);
                    }
                    ungetc(curr_ch, stdin);
                    string_char_push(output_data, (char) T_FLOAT);//insert type to the end of the output string
                    return;
                }
            }
            break;

            case float64exp : {
                if (curr_ch == '+' || curr_ch == '-') {
                    string_char_push(output_data, (char)curr_ch);
                    state = float64exp_sign;
                }
                else if (isdigit(curr_ch)) {
                    string_char_push(output_data, (char)curr_ch);
                    state = float64;
                }
                else if (curr_ch == '_') {
                    exit(LEXICAL_ERR); //'_' must separate successive digits
                }
                else {
                    ungetc(curr_ch, stdin);
                    exit(LEXICAL_ERR);
                }
            }
            break;

            case float64exp_sign : {
                if (isdigit(curr_ch)) {
                    string_char_push(output_data, (char)curr_ch);
                    state = float64;
                }
                else if (curr_ch == '_') {
                    exit(LEXICAL_ERR); //'_' must separate successive digits
                }
                else {
                    ungetc(curr_ch, stdin);
                    exit(LEXICAL_ERR);
                }
            }
            break;
            
            case float64 : {
                if (isdigit(curr_ch)) {
                    string_char_push(output_data, (char)curr_ch);
                }
                else if(curr_ch == '_') {
                    if (underscore){
                        exit(LEXICAL_ERR); //'_' must separate successive digits
                    }
                    underscore = true; //last read char was underscore
                }
                else {
                    if (underscore) {
                        exit(LEXICAL_ERR);
                    }
                    ungetc(curr_ch, stdin);
                    string_char_push(output_data, (char) T_FLOAT);//insert type to the end of the output string
                    return;
                }
            }
            break;
            default:
                exit(LEXICAL_ERR);
        }
        if (curr_ch != '_') {
            underscore = false;
        }
    }
}

uint8_t keyword_idf(string_t *output_data, char first)
{
	int curr_char;
	string_char_push(output_data,first);
	while((curr_char = getchar()) != EOF && (isalnum(curr_char) || curr_char == '_'))
		string_char_push(output_data, (char)curr_char);
    
	
	uint8_t ret_val;
	if((ret_val = is_key_word(string_read(*output_data))) == NOT_KEYWORD)
	{
        // it is a indetifier
		if(isspace(curr_char) && curr_char != '\n')
        {
            //skip spaces after the identifier
			while(isspace(curr_char = getchar()) && curr_char != EOF && curr_char != '\n'); // while without body
        }
		
		
		if(curr_char == '(') // the first non white character is ( -> function is read
		{
			return T_FUNC; // return function
		}	
		else
        {
			ungetc(curr_char, stdin);
			return T_IDF; // return identifier
        }
	}
	else // keyword
	{
        ungetc(curr_char, stdin);
		string_reset(*output_data);
		string_char_push(output_data, ret_val);
		if(ret_val == T_INT || ret_val == T_BOOL || ret_val == T_STRING || ret_val == T_FLOAT)
			return T_TYPE;
		else if(ret_val == TRUE || ret_val == FALSE)
		{	
			string_char_push(output_data, T_BOOL);
			return T_NUM; // return constant
		}
		else
			return T_KEY; // return keyword
	}			
	
}
