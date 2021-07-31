#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
void operator();
void operand();
void close_bracket();
void open_bracket();

static char operators[] = {'+','+','+', '-','-','-','*', '*','/'};
static char *operands[] ={"o","p","q","6","(+2)","(-3)","(-(-1))","foo0(7)", "foo1(4)", "foo2(6)"};
static char *cmp_operators[] = { "<=", ">=", "<", ">", "==", "!=" };
static char *logic_operators[] = {"||", "&&"};

struct brackets {
        int brackets_to_open;
        int brackets_to_close;
        bool operator_was_used;
        bool can_use_cmp;
        bool can_use_logic;
        bool need_use_cmp;
        bool negation_was_used;
        bool use_logic;
        char curr_operator;
        bool gen_func_call;
}BR;

void expr_in_bracket()
{
        rand()%2 ? printf("(-%s)%c", operands[rand()% (!BR.gen_func_call ? 7:10)], operators[rand()%9]) : printf("(+%s)%c",operands[rand()% (!BR.gen_func_call ? 7:10)], operators[rand()%9]);
}
void unary_negation()
{
        do{
                putchar('!');
        }while(rand()%4 > 1);

        putchar('(');
        BR.negation_was_used = true;
        BR.need_use_cmp = true;
}


void operator()
{
        BR.operator_was_used = true;
        if(BR.use_logic && BR.brackets_to_close == 0 && rand()%2)
        {

                if(BR.can_use_cmp)
                {
                        printf(" %s ", cmp_operators[rand()%6]);
                        BR.can_use_cmp = false;
                        BR.need_use_cmp = false;
                        BR.can_use_logic = true;

                }
                else if(BR.can_use_logic)
                {
                        if(BR.negation_was_used)
                        {
                                putchar(')');
                                BR.negation_was_used = false;
                        }
                        printf(" %s ", logic_operators[rand()%2]);
                        BR.can_use_cmp = true;
                        BR.need_use_cmp = true;
                        BR.can_use_logic = false;
                        if(rand()%2)
                        unary_negation();
                }
        }
        else
        {
                putchar((BR.curr_operator = operators[rand()%9]));
                if(rand()%2)
                {
                        (BR.curr_operator == '/' || BR.curr_operator == '*') ? putchar(operators[rand()%6]) : expr_in_bracket();
                }
        }
        rand() % 2 ? operand() : open_bracket();
}

void close_bracket()
{
        if(BR.brackets_to_close > 0 && BR.operator_was_used)
        {
                putchar(')');
                BR.brackets_to_close--;
        }
        if(BR.brackets_to_close == 0 && BR.brackets_to_open == 0 && !BR.need_use_cmp)
        {
                if(rand() % 3 > 0)
                        operator();
                else
                {
                        if(BR.negation_was_used)
                                putchar(')');
                        return;
                }
        }
        else if(BR.brackets_to_close > 0)
                rand() % 2 ? operator() : close_bracket();
        else
                operator();
}

void open_bracket()
{
        if(BR.brackets_to_open == 0)
                operand();
        else
        {
                BR.operator_was_used = false;
                BR.brackets_to_open--;
                BR.brackets_to_close++;
                putchar('(');
                rand() % 2 ? operand() : open_bracket();
        }
}

void operand()
{
        printf("%s", operands[rand()% (!BR.gen_func_call ? 7 : 10)]);

        if(BR.brackets_to_close == 0 && BR.brackets_to_open == 0 && BR.operator_was_used && !BR.need_use_cmp && rand()%2 > 0)
        {
                if(BR.negation_was_used)
                        putchar(')');
                return;
        }
        else
                rand() % 2 ? operator() : close_bracket();


}

int generate_expression(bool use_logic,bool gen_func_call)
{
        static int var_count = 0;
        BR.brackets_to_open = (rand() % 4)+4;

        BR.brackets_to_close = 0;

        BR.operator_was_used = false;

        BR.can_use_cmp = true;

        BR.use_logic = use_logic; //FALSE --> turn off the logic operators

        BR.can_use_logic = false;
	
	if(use_logic)
        	BR.need_use_cmp =true;
	else
		BR.need_use_cmp =false;

        BR.negation_was_used = false;

        BR.gen_func_call =   gen_func_call; //FALSE --> no func calls in expression;
        
	printf("\tif ");
	//printf("\tx%d := ", var_count); //assignement expression
        if(rand()%2 && BR.use_logic)
                unary_negation();
        if(rand()%2)
                operand();
        else
                open_bracket();
	printf(" {\n\t\tprint(\":D%d\",\"\\n\")\n\t}",var_count);
        putchar('\n');
        
	return var_count++;
}


void generate_function(bool gen_int)
{
        static int func_count=0;
        printf("func foo%d(n int) (%s) {\n", func_count, gen_int ? "int" : "float64");
        
	//printf("\to,p,q:=1.2,2.3,3.4\n");
        //int varnum = generate_expression(false, true);
        //printf("\tprint(o,p,q,\"\\n\")\n");
	printf("\tif n == 1 {\n\t\treturn 2\n\t}\n");
        printf("\treturn foo%d(n-1)\n}\n\n", func_count);
        func_count++;
}


int main()
{
	sleep(1);
        //int varnum;
        printf("package main\n\n");
        for(int i = 0; i<3;i++)
        	generate_function(true);

        printf("func main() {\n");
        printf("\to,p,q :=1,3,7\n\n");

        srand(time(NULL));

        for(int i = 0; i < 50; i++)
        {
                //varnum = 
		generate_expression(true, true);//first argument - logic, second one - function calls
                //printf("\tprint(x%d,\"\\n\")\n",varnum);
                putchar('\n');

        }
        printf("\tprint(o,p,q,\"\\n\")\n");
        putchar('}');
        putchar('\n');
}

