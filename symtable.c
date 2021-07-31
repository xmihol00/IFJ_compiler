
//=========================================================================================================
// File:        symtable.c
// Case:        VUT, FIT, IFJ, compiler project
// Date:        9. 12. 2020
// Author(s):   David Mihola - functions needed for optimalization, Lukas Foltyn - compulsory functions
// Contac(s):   xmihol00@stud.fit.vutbr.cz, xfolty17@stud.fit.vutbr.cz
// Compiled:    gcc version 9.3.0 (Ubuntu 9.3.0-17ubuntu1~20.04)
// Description: File containing constant and functions definitions maniging the table of symbols
//==========================================================================================================

#include "symtable.h"

#define HT_FUNC_SIZE 97
#define PARTIAL_HT_SIZE 71
#define PARTIAL_HT_COUNT 16
#define INIT_HASH_VECTOR_SIZE 4
#define FRAME_SHIFT 0

enum var_states {
	VAR_HAS_NO_VALUE = 0,
	VAR_HAS_VALUE = 1,
	VAR_IS_ASSIGNED = 2,
	VAR_HAS_CHANGED = 3,
	VAR_HAS_RENEWED_VALUE = 4,
};

/*------------- HT-variables ------------ */

/**
 * @struct representation of a program variable
**/
typedef struct var_htab_item
{
    char *name;         	/// the name of a variable
    int scope;        		/// stores the scope of declaration, or has -1 value if not active 
    bool frame;				/// the frame of an variable (LF == false GF == true)
	uint8_t state;      	/// its state
    uint8_t type;    		/// its type
	vect_uint8_t has_value; /// indicates if the variable has known value on a given scope 
    vect_var_val_t values;  /// its value, if it has any

    struct var_htab_item *next; /// next item
}*T_var_item;

/// the partial variable hash table for each function
typedef T_var_item * T_partial_var_htab;
typedef T_var_item ** T_partial_ht_arr;

/**
 * @struct variable hash table
**/
struct htab_var{
    int64_t size;   				/// the current size of the hash table
    int64_t pos;       				/// index of current partion hash table        
    T_partial_ht_arr partial_htabs; /// an array of partial hash tables

    T_var_item special_var; 		/// special variable '_' 
        
    uint8_t scope_vect_size;
    vect_uint8_t *hashes_scope_to_rotate; /// an array of vectors with hashes that need to be rotated after closing scope 
}G_var_htab;       //vector of hash tables

/* ---------- HT-functions -----------*/

/**
 * @struct representation of a program function
**/
typedef struct func_htab_item
{
    char *name;                 		/// the name of the function
    uint8_t state;                		/// the state of the function: declared/defined/inline...

    vect_htab_var_t params;        		/// the parameters 

    vect_htab_var_t ret_vals;     		/// the return values
    vect_uint8_t ret_val_types;     	/// the types of the function return values

	vect_htab_func_t called_functions;	/// called functions
    bool visited;						/// to prevent cycles when searching for recursive functions
    bool is_not_recursive;

    T_var_item* partial_var_ht;
    struct func_htab_item *next;

}*T_func_item;

typedef T_func_item * T_func_htab;

struct func_htab
{
	T_func_htab hash_table; //pointer to the tables
	vect_htab_var_t build_in_vars; /// variables for all build in functions that are not in hash_table	
}G_func_htab; //global hash table for functions

/* ------------------ static functions - not visible to other files --------------- */

static unsigned htab_hash_fun(const char *str)
{
    unsigned h =0;    
    const unsigned char *p;
    for(p=(const unsigned char*)str; *p!='\0'; p++)
        h = 65599*h + *p;
    return h;
}

static void resize_partial_htabs()
{
	unsigned int bytes_to_copy= G_var_htab.size*sizeof(T_partial_var_htab);
	G_var_htab.size *= 2;
	T_partial_ht_arr bigger_array = calloc(G_var_htab.size, sizeof(T_partial_var_htab));
	if(bigger_array == NULL)
		exit(MEM_ERR);
	memcpy(bigger_array,G_var_htab.partial_htabs,bytes_to_copy); //copying the content to newly allocated memory
	free(G_var_htab.partial_htabs);
	G_var_htab.partial_htabs = bigger_array;
}

static void resize_hashes_scope_to_rotate()
{
	unsigned int bytes_to_copy = G_var_htab.scope_vect_size * sizeof(vect_uint8_t);
	G_var_htab.scope_vect_size *= 2;
	vect_uint8_t *bigger_array = calloc(G_var_htab.scope_vect_size, sizeof(vect_uint8_t));
	if(bigger_array == NULL)
		exit(MEM_ERR);
	memcpy(bigger_array, G_var_htab.hashes_scope_to_rotate, bytes_to_copy); //copying the content to newly allocated memory
	free(G_var_htab.hashes_scope_to_rotate);
	G_var_htab.hashes_scope_to_rotate = bigger_array;
}	

static T_partial_var_htab get_partial_htab()
{
	if(++G_var_htab.pos == G_var_htab.size)
		resize_partial_htabs();
	
	if((G_var_htab.partial_htabs[G_var_htab.pos] = calloc(PARTIAL_HT_SIZE,sizeof(T_var_item))) != NULL)//allocating new partial HT 
		return G_var_htab.partial_htabs[G_var_htab.pos]; //return it to function
	else
		exit(MEM_ERR);
}

static T_var_item create_var_ht(const char* name, const int scope)
{
	static unsigned unique_var_id = 0;
	char buffer[11];
	sprintf(buffer, "&%d", ++unique_var_id);

	T_var_item new = calloc (1,sizeof(struct var_htab_item));
	if(new == NULL)
		exit(MEM_ERR);
	size_t str_lenght = strlen(name); 
	new->name = calloc (str_lenght+strlen(buffer)+2, sizeof(char)); //+1 for zero character, +1 for special character 
	if(new->name == NULL)
		exit(MEM_ERR);

	strcpy(new->name, name);
	strcpy(new->name + str_lenght, buffer);
	new->scope = scope;
	
	return new;
}

static T_func_item create_func_ht(const char* name)
{
	T_func_item new = calloc (1,sizeof(struct func_htab_item));
	if(new == NULL)
		exit(MEM_ERR);
	size_t str_lenght = strlen(name);
	new->name = malloc (str_lenght+1);
	if(new->name == NULL)
		exit(MEM_ERR);
	strcpy(new->name,name);
	new->params = NULL;
	new->ret_vals = NULL;
	new->ret_val_types = NULL;
	new->next = NULL;
	new->partial_var_ht = NULL;
	return new;

}

static void insert_one_build_in_function(htab_func_t t, unsigned index)
{	
	if(G_func_htab.hash_table[index] == NULL)
		G_func_htab.hash_table[index] = t;
	else
	{	T_func_item temp = G_func_htab.hash_table[index];
		while(temp->next != NULL)
			temp = temp->next;
		temp->next = t;
	}
}	

static void insert_build_in_functions()
{
	char *func_name[] = {"inputs", "inputb", "inputi", "inputf", "print", "int2float", "float2int", "len", "substr", "ord", "chr" };

	T_func_item build_func_arr[11];
	for(int i = 0; i < 11; i++)
		build_func_arr[i] = create_func_ht(func_name[i]);
	
	//inputs
	add_func_ret_val_type(build_func_arr[0], T_STRING, false);
	add_func_ret_val_type(build_func_arr[0], T_INT, false);
	build_func_arr[0]->state = F_DEF;

	//inputb
	add_func_ret_val_type(build_func_arr[1], T_BOOL, false);
	add_func_ret_val_type(build_func_arr[1], T_INT, false);
	build_func_arr[1]->state = F_DEF;

	//inputi
	add_func_ret_val_type(build_func_arr[2], T_INT, false);
	add_func_ret_val_type(build_func_arr[2], T_INT, false);
	build_func_arr[2]->state = F_DEF;

	//inputf
	add_func_ret_val_type(build_func_arr[3], T_FLOAT, false);
	add_func_ret_val_type(build_func_arr[3], T_INT, false);
	build_func_arr[3]->state = F_DEF;

	//print TODO
	build_func_arr[4]->params = vect_htab_var_const();
	build_func_arr[4]->state = F_DEF;
	
	//int2float
	T_var_item var = create_var_ht("i",0);
	vhtv_push(&G_func_htab.build_in_vars,var); 
        set_var_type(var, T_INT);
	add_func_parameter(build_func_arr[5], var);
	
	add_func_ret_val_type(build_func_arr[5], T_FLOAT, false);
	build_func_arr[5]->state = F_DEF;
	
	//float2int
	var = create_var_ht("f",0);
	vhtv_push(&G_func_htab.build_in_vars,var);
        set_var_type(var, T_FLOAT);
	add_func_parameter(build_func_arr[6], var);
	
	add_func_ret_val_type(build_func_arr[6], T_INT, false);
	build_func_arr[6]->state = F_DEF;
	
	//len
	var = create_var_ht("s",0);
	vhtv_push(&G_func_htab.build_in_vars,var);
        set_var_type(var, T_STRING);
	add_func_parameter(build_func_arr[7], var);
	
	add_func_ret_val_type(build_func_arr[7], T_INT, false);
	build_func_arr[7]->state = F_DEF;

	//substr
	var = create_var_ht("?temp",0);
	var->name[6] = '3';
	var->name[7] = '\0';
	var->frame = true;
	vhtv_push(&G_func_htab.build_in_vars,var);
        set_var_type(var, T_STRING);
	add_func_parameter(build_func_arr[8], var);
	var = create_var_ht("?temp",0);
	var->name[6] = '2';
	var->name[7] = '\0';
	var->frame = true;
	vhtv_push(&G_func_htab.build_in_vars,var);
        set_var_type(var, T_INT);
	add_func_parameter(build_func_arr[8], var);
	var = create_var_ht("?temp",0);
	var->name[6] = '1';
	var->name[7] = '\0';
	var->frame = true;
	vhtv_push(&G_func_htab.build_in_vars,var);
        set_var_type(var, T_INT);
	add_func_parameter(build_func_arr[8], var);
	
	add_func_ret_val_type(build_func_arr[8], T_STRING, false);
	add_func_ret_val_type(build_func_arr[8], T_INT, false);
	build_func_arr[8]->state = F_DEF;
	
	//ord
	var = create_var_ht("?temp",0);
	var->name[6] = '2';
	var->name[7] = '\0';
	var->frame = true;
	vhtv_push(&G_func_htab.build_in_vars,var);
        set_var_type(var, T_STRING);
	add_func_parameter(build_func_arr[9], var);
	var = create_var_ht("?temp",0);
	var->name[6] = '1';
	var->name[7] = '\0';
	var->frame = true;
	vhtv_push(&G_func_htab.build_in_vars,var);
        set_var_type(var, T_INT);
	add_func_parameter(build_func_arr[9], var);
	
	add_func_ret_val_type(build_func_arr[9], T_INT, false);
	add_func_ret_val_type(build_func_arr[9], T_INT, false);
	build_func_arr[9]->state = F_DEF;
	
	//chr
	var = create_var_ht("?temp",0);
	var->name[6] = '1';
	var->name[7] = '\0';
	var->frame = true;
	vhtv_push(&G_func_htab.build_in_vars,var);
        set_var_type(var, T_INT);
	add_func_parameter(build_func_arr[10], var);
	
	add_func_ret_val_type(build_func_arr[10], T_STRING, false);
	add_func_ret_val_type(build_func_arr[10], T_INT, false);
	build_func_arr[10]->state = F_DEF;

	for(int i=0; i < 11; i++) // load to HT
	{
		insert_one_build_in_function(build_func_arr[i],htab_hash_fun(func_name[i])%HT_FUNC_SIZE);
	
	}
}	

/* --------------------------- functions for hash tables working with functions ------------------- */

void create_compiler_variables()
{
	T_var_item var = create_var_ht("?temp", 0);
	vhtv_push(&G_func_htab.build_in_vars, var);
	var->frame = true;
	var = create_var_ht("?temp", 0);
	vhtv_push(&G_func_htab.build_in_vars, var);
	var->frame = true;
	var = create_var_ht("?temp", 0);
	vhtv_push(&G_func_htab.build_in_vars, var);
	var->frame = true;
}

htab_var_t get_tmp_var(int64_t pos)
{
	return vhtv_get_pos(G_func_htab.build_in_vars, pos);
}

void init_func_ht()
{
	if((G_func_htab.hash_table = calloc (HT_FUNC_SIZE,sizeof(T_func_item))) == NULL)
		exit(MEM_ERR);

	G_func_htab.build_in_vars = vect_htab_var_const();
	
	create_compiler_variables();
	insert_build_in_functions();
}

htab_func_t find_func_ht(const char* key)
{
	unsigned index = htab_hash_fun(key)%HT_FUNC_SIZE;
	for(T_func_item temp = G_func_htab.hash_table[index]; temp != NULL; temp= temp->next)
		if(!strcmp(temp->name,key))
			return temp;

	return NULL;
}

htab_func_t insert_func_ht(const char* key)
{
	unsigned index = htab_hash_fun(key) % HT_FUNC_SIZE;
	if(G_func_htab.hash_table[index] == NULL)
	{	
		G_func_htab.hash_table[index] = create_func_ht(key);
		return G_func_htab.hash_table[index];
	}	
	else
	{	T_func_item temp;
		for(temp = G_func_htab.hash_table[index]; temp->next != NULL; temp = temp->next)
			if(!strcmp(temp->name, key))
				exit(SEMANTIC_DEF_ERR); // redefinition of a function

		if(!strcmp(temp->name,key))
			exit(SEMANTIC_DEF_ERR); // redefinition of a function
		temp->next = create_func_ht(key);
		return temp->next;
	}
	
}

void dispose_func_ht()
{
	for(unsigned i = 0; i < HT_FUNC_SIZE; i++)
	{
		for(T_func_item temp = G_func_htab.hash_table[i]; temp != NULL; temp = G_func_htab.hash_table[i])
		{
			G_func_htab.hash_table[i] = G_func_htab.hash_table[i]->next;
			free(temp->name);
			free(temp);	
		}			
	}
	T_var_item temp1;
	while(!vhtv_is_empty(G_func_htab.build_in_vars)) //dispose all variables that were used for buildin functions, but not added to HT for variables
	{
		temp1 = vhtv_pop(G_func_htab.build_in_vars);
		free(temp1->name);
		free(temp1);
	}
	discard_htab_var_vector_free(&G_func_htab.build_in_vars);	
	free(G_func_htab.hash_table);	
}

void attache_partial_htab(htab_func_t t)
{
	t->partial_var_ht = get_partial_htab();
}	

void add_func_parameter(htab_func_t t, htab_var_t v)
{
	if(t->params == NULL)
		t->params = vect_htab_var_const();
	vhtv_push(&t->params,v);
}

void add_func_call(htab_func_t f, htab_func_t called)
{
	if (f->called_functions == NULL)
	{
		f->called_functions = vect_htab_func_const();
	}
	vfht_push_unique(&f->called_functions, called);
}

void add_func_param_type(htab_func_t t, uint8_t type)
{
	T_var_item item = vhtv_get_last(t->params);
	item->type = type;
}

void add_func_ret_val(htab_func_t t, htab_var_t v)
{
	if(t->ret_vals == NULL)
		t->ret_vals = vect_htab_var_const();
	vhtv_push(&t->ret_vals,v);
}

void add_func_ret_val_type(htab_func_t t, uint8_t type, bool has_name)
{
	if (has_name)
	{
		T_var_item item = vhtv_get_last(t->ret_vals);
		item->type = type;
	}

	if (t->ret_val_types == NULL)
		t->ret_val_types = vect_uint8_const();
	vu8_push(&t->ret_val_types, type);
}

uint8_t get_func_ret_val_signature(htab_func_t t)
{
	if (t->ret_val_types)
	{
		if (t->ret_vals == NULL)
			return NOT_NAMED_S;
		
		return NAMED_S;
	}

	return NOT_SET_S;
}

vect_htab_var_t get_params(htab_func_t t)
{
	return t->params;
}

vect_htab_var_t get_ret_vals(htab_func_t t)
{
	return t->ret_vals;
}

vect_uint8_t get_ret_val_types(htab_func_t t)
{
	return t->ret_val_types;
}

void set_func_state(htab_func_t t , uint8_t state)
{
	t->state = state;
}

uint8_t get_func_state(htab_func_t t)
{
	return t->state;
}

void set_func_effect(htab_func_t t, bool visited)
{
	t->visited = visited;
}

bool get_func_effect(htab_func_t t)
{
	return t->visited;
}

void set_func_is_recursive(htab_func_t t, bool is_not_recursive)
{
	t->is_not_recursive = is_not_recursive;
}

bool func_is_recursive(htab_func_t t)
{
	return !t->is_not_recursive;
}

bool func_is_not_recursive(htab_func_t t)
{
	return t->is_not_recursive;
}

const char* get_func_name(htab_func_t t)
{
	return t->name;
}	

void all_func_have_definition()
{
	bool main_found = false;
	for(unsigned i = 0; i < HT_FUNC_SIZE; i++)
	{
		for(T_func_item temp = G_func_htab.hash_table[i]; temp != NULL; temp = temp->next)
		{
			if(temp->state != F_DEF)
				exit(SEMANTIC_DEF_ERR); // undefined function call
			if(!strcmp(temp->name, "main"))
			{
				if (temp->params != NULL || temp->ret_vals != NULL || temp->ret_val_types != NULL)
					exit(SEMANTIC_PARAM_COUNT_ERR); // main function with incorrect signature
				
				main_found = true;
			}
		}			
	}
	if (!main_found)
	{
		exit(SEMANTIC_DEF_ERR); // not existing main function
	}
}	

void find_recursive_functions()
{
	T_func_item tmp1, tmp2;
	// data structure representing the graph (vector of vector of functions)
	vect_dbl_func_t vdfht_tmp = vect_dbl_func_const();
	vect_htab_func_t func_vect; // single vector of functions
	htab_func_t function; // function

	for (int64_t i = 0; i < HT_FUNC_SIZE; i++) // for each bucket in the hash table
	{
		tmp2 = G_func_htab.hash_table[i];
		while (tmp2 != NULL) //until the linked list of a given bucket is not empty
		{
			for (int64_t j = 0; j < HT_FUNC_SIZE; j++) // for each bucket in the hash table
			{
				tmp1 = G_func_htab.hash_table[j];
				while (tmp1 != NULL) //until the linked list of a given bucket is not empty
				{
					tmp1->visited = false; // set the flag of all functions as not visited
					tmp1 = tmp1->next;
				}
			}

			tmp2->visited = true; // set the currently searched function to visited
			tmp2->is_not_recursive = true; // assume, that the function is not recursive
			if (tmp2->called_functions != NULL)
			{
				vdfht_push(&vdfht_tmp, tmp2->called_functions); // store the called functions of the search function
				while((func_vect = vdfht_get_next(vdfht_tmp)) != NULL) // until there are nodes to be searched
				{
					// compare all called function from a given node
					for (int64_t j = vfht_pos(func_vect); j >= 0; j--)
					{
						function = vfht_get_offset(func_vect, j);
						if (function == tmp2)
						{
							// circle found, function is recursive
							tmp2->is_not_recursive = false;
							reset_dbl_func_vector(vdfht_tmp);
							function->visited = true;
							break;
						}

						if (!function->visited)
						{
							// if the function wasnt visited previously, add its function calls
							if (function->called_functions != NULL)
							{
								vdfht_push(&vdfht_tmp, function->called_functions);
							}
							function->visited = true; // mark the function as visited
						}				
					}
				}
				reset_dbl_func_vector(vdfht_tmp); // reset the double vecotor
			}	
			tmp2 = tmp2->next; // continue to the next node
		}
	}
	discard_dbl_func_vector(&vdfht_tmp);
}

/* ------------------- functions for hash tables working with variables ----------------------*/
void start_scope(uint32_t started_scope)
{
	if(started_scope == G_var_htab.scope_vect_size)
		resize_hashes_scope_to_rotate();
	if(started_scope >= INIT_HASH_VECTOR_SIZE)
		G_var_htab.hashes_scope_to_rotate[started_scope] = vect_uint8_const(); 

}

void end_scope(int64_t ended_scope)
{
	while(!vu8_is_empty(G_var_htab.hashes_scope_to_rotate[ended_scope]))
	{
		uint8_t index = vu8_pop(G_var_htab.hashes_scope_to_rotate[ended_scope]);
		
		for(;G_var_htab.partial_htabs[G_var_htab.pos][index]->scope == ended_scope; G_var_htab.partial_htabs[G_var_htab.pos][index]= G_var_htab.partial_htabs[G_var_htab.pos][index]->next)
			G_var_htab.partial_htabs[G_var_htab.pos][index]->scope = -1; //setting all variables in just closed scope to inactive	
	}
	if(ended_scope > INIT_HASH_VECTOR_SIZE)
		discard_uint8_vector(&G_var_htab.hashes_scope_to_rotate[ended_scope]);
		
}	

void init_global_ht()
{
	G_var_htab.size = PARTIAL_HT_SIZE;
	G_var_htab.scope_vect_size = INIT_HASH_VECTOR_SIZE;
	G_var_htab.pos = -1;
	G_var_htab.partial_htabs = calloc(PARTIAL_HT_SIZE, sizeof(T_partial_var_htab));
	
	if(G_var_htab.partial_htabs == NULL)
		exit(MEM_ERR);

	G_var_htab.hashes_scope_to_rotate = calloc(G_var_htab.scope_vect_size, sizeof(vect_uint8_t));//vector for indexes in HT, where were recenlty inserted variables
	
	if(G_var_htab.hashes_scope_to_rotate != NULL)
	{
		for(unsigned i = 0; i < INIT_HASH_VECTOR_SIZE;i++)
			G_var_htab.hashes_scope_to_rotate[i] = vect_uint8_const();
	}
	else	
		exit(MEM_ERR);
	G_var_htab.special_var = calloc(1,sizeof(struct var_htab_item));
	if(G_var_htab.special_var == NULL)
		exit(MEM_ERR);
	
	G_var_htab.special_var->name = "GF@?_&";

	G_var_htab.special_var->scope = 0;
}

htab_var_t insert_var_ht(const char* key, const int scope) 
{
	if(key[0] == '_' && key[1] == '\0')
	      exit(SEMANTIC_DEF_ERR); // redefinition of a special variable '_'

	size_t str_lenght = strlen(key);
	unsigned index = htab_hash_fun(key)%PARTIAL_HT_SIZE;
	if(G_var_htab.partial_htabs[G_var_htab.pos][index] == NULL)
	{
		vu8_push(&G_var_htab.hashes_scope_to_rotate[scope], (uint8_t)index);	
		G_var_htab.partial_htabs[G_var_htab.pos][index] = create_var_ht(key,scope);
		return(G_var_htab.partial_htabs[G_var_htab.pos][index]->next = G_var_htab.partial_htabs[G_var_htab.pos][index]);
	}
	
	else
	{	T_var_item temp;
		for(temp = G_var_htab.partial_htabs[G_var_htab.pos][index]; temp->next != G_var_htab.partial_htabs[G_var_htab.pos][index]; temp = temp->next)
		{
			if(!strncmp(temp->name,key,str_lenght) && temp->scope == scope)
				exit(SEMANTIC_DEF_ERR); // redefinition of exisitng variable

		}
		if(!strncmp(temp->name,key, str_lenght) && temp->scope == scope)
				exit(SEMANTIC_DEF_ERR); // redefinition of exisitng variable
		
		vu8_push(&G_var_htab.hashes_scope_to_rotate[scope], (uint8_t)index);
		temp->next = create_var_ht(key,scope);
		temp->next->next = G_var_htab.partial_htabs[G_var_htab.pos][index];
 	        return(G_var_htab.partial_htabs[G_var_htab.pos][index] = temp->next);

	}
}	

htab_var_t find_var_ht(const char* key)
{	
	if(key[0] == '_' && key[1] == '\0')
		return G_var_htab.special_var;

	size_t str_lenght = strlen(key);
	unsigned index = htab_hash_fun(key)%PARTIAL_HT_SIZE;
	if(G_var_htab.partial_htabs[G_var_htab.pos][index] != NULL)
	{
		if(!strncmp(G_var_htab.partial_htabs[G_var_htab.pos][index]->name, key,str_lenght))
		{	
			if(G_var_htab.partial_htabs[G_var_htab.pos][index]->scope != -1)	
				return G_var_htab.partial_htabs[G_var_htab.pos][index];
			else
				return NULL;
		}	

		for(T_var_item temp = G_var_htab.partial_htabs[G_var_htab.pos][index]->next; temp != G_var_htab.partial_htabs[G_var_htab.pos][index]; temp= temp->next)
			if(!strncmp(temp->name,key, str_lenght))
			{	
				if(temp->scope != -1) // variable is not in closed scope
					return temp;
				else
					return NULL;
			}	
	}
	return NULL;	
}

void dispose_global_ht()
{	
	T_var_item temp1,temp2;
	for(int64_t j = 0; j < G_var_htab.size; j++ )//for every partial HT
	{
		if(G_var_htab.partial_htabs[j])
		{	
			for(unsigned i = 0; i < PARTIAL_HT_SIZE; i++)//for every index is partial HT
			{
				if(G_var_htab.partial_htabs[j][i] != NULL)//one particural hash - removing circural linked list
				{
					for(temp1 = G_var_htab.partial_htabs[j][i], temp2 = temp1->next; temp2 != G_var_htab.partial_htabs[j][i];temp1=temp2, temp2=temp2->next)
					{
						if (temp1->type == T_STRING && temp1->values)
						{
							vvv_free_reset_strings(temp1->values);
						}
						free(temp1->name);
						free(temp1);
					}
					if (temp1->type == T_STRING && temp1->values)
					{
						vvv_free_reset_strings(temp1->values);
					}
					free(temp1->name);
					free(temp1);
				}
			}
			free(G_var_htab.partial_htabs[j]);
		}	
	}
	free(G_var_htab.special_var);
	free(G_var_htab.partial_htabs);
	free(G_var_htab.hashes_scope_to_rotate);	
}

htab_var_t find_next_def_var(htab_var_t var)
{
	if (var == G_var_htab.special_var)
	{
		return NULL;
	}
	
	htab_var_t tmp = var;
	unsigned i = 1;
	while (var->name[i] != '&')
	{
		i++;
	}

	tmp = tmp->next;
	while (tmp != var)
	{
		if (!strncmp(var->name, tmp->name, i) && tmp->state == V_DEF && tmp->scope >= 0)
		{
			return tmp;
		}
		tmp = tmp->next;
	}
	return NULL;
}

void set_var_frame(htab_var_t var, bool frame)
{
	var->frame = frame;
}

bool get_var_frame(htab_var_t var)
{
	return var->frame;
}

void set_var_type(htab_var_t t, uint8_t type)
{
	t->type = type;
}

uint8_t get_var_type(htab_var_t t)
{
	return t->type;
}	

void set_var_state(htab_var_t t,uint8_t state)
{
	t->state = state;
}

void unset_var_value(htab_var_t var)
{
	if (var->has_value)
	{
		if (vu8_get_last(var->has_value))
		{
			vu8_set_last(var->has_value, VAR_HAS_NO_VALUE);
			if (var->type == T_STRING)
			{
				vvv_free_last_string(var->values);
			}
			vvv_pop(var->values);
		}
	}
}

void assign_constants_before_scope()
{
	htab_var_t var;
	char *tmp;
	for (uint8_t i = 0; i < PARTIAL_HT_SIZE; i++)
	{
		var = G_var_htab.partial_htabs[G_var_htab.pos][i];
		while (var != NULL)
		{
			if (var->has_value && vu8_get_last(var->has_value) & 1 && vvv_pos(var->values) == 0)
			{
				ltype_insert(T_ADDRESS_ASSGN);
				ltype_insert(T_MOV);
				lvar_insert(var);
				switch(var->type)
				{
					case T_INT:
						lint_insert(vvv_get_last(var->values).integer);
						break;

					case T_BOOL:
						lbool_insert(vvv_get_last(var->values).boolean);
						break;

					case T_STRING:
						tmp = malloc(strlen(vvv_get_last(var->values).string) + 1);
						if (tmp == NULL)
						{
							exit(MEM_ERR);
						}
						strcpy(tmp, vvv_get_last(var->values).string);
						lstring_insert(tmp);
						break;

					case T_FLOAT:
						lfloat_insert(vvv_get_last(var->values).float64);
						break;

					default:
						break;
				}
				ltype_insert(T_ADR_ASSGN_END);
				vu8_set_last(var->has_value, VAR_IS_ASSIGNED);
			}		
			var = var->next;
			if (var == G_var_htab.partial_htabs[G_var_htab.pos][i])
			{
				break;
			}
		}
	}
}

var_values_t get_var_value(htab_var_t var)
{
	return vvv_get_last(var->values);
}

void set_var_value(htab_var_t var, var_values_t value, uint8_t type)
{
	if (var->type == 0 || var->type == type)
	{
		if (var->values == NULL)
		{
			var->values = vect_var_val_const();
			var->has_value = vect_uint8_const();
			vvv_push(&var->values, value);
			vu8_push(&var->has_value, VAR_HAS_VALUE);
		}
		else
		{
			if (vu8_get_last(var->has_value))
			{
				if (var->type == T_STRING)
				{
					if (!strcmp(vvv_get_last(var->values).string, value.string))
					{
						free(value.string);
						return;
					}

					vvv_free_last_string(var->values);
				}
				vvv_change_last(var->values, value);
			}
			else
			{
				if (var->type == T_STRING && vu8_pos(var->has_value) > 0 && vu8_get_offset(var->has_value, 1))
				{
					if (!strcmp(vvv_get_last(var->values).string, value.string))
					{
						free(value.string);
						vvv_push_copy(&var->values);
						vu8_set_last(var->has_value, VAR_HAS_RENEWED_VALUE);
						return;
					}
				}
				vvv_push(&var->values, value);
			}
			vu8_set_last(var->has_value, VAR_HAS_VALUE);
		}
		var->type = type;
	}
	else
	{
		exit(SEMANTIC_EXPR_ERR); //assignmen of a value with different type
	}
}

void copy_vars_to_new_scope()
{
	htab_var_t var;
	for (uint8_t i = 0; i < PARTIAL_HT_SIZE; i++) // for every bucket of the hash table
	{
		var = G_var_htab.partial_htabs[G_var_htab.pos][i];
		while (var != NULL)
		{
			if (var->has_value) // if the variable has value
			{
				if (vu8_get_last(var->has_value))
				{
					// copy the value from previous scope
					vvv_push_copy(&var->values);
					vu8_push(&var->has_value, VAR_IS_ASSIGNED);
				}
				else
				{
					// add that the variable has no value also on the new scope
					vu8_push(&var->has_value, VAR_HAS_NO_VALUE);
				}
			}
			var = var->next;
			// circular list
			if (var == G_var_htab.partial_htabs[G_var_htab.pos][i])
			{
				break;
			}
		}
	}
}

void copy_vars_reopened_scope()
{
	htab_var_t var;
	for (uint8_t i = 0; i < PARTIAL_HT_SIZE; i++) // for each bucket of the table
	{
		var = G_var_htab.partial_htabs[G_var_htab.pos][i];
		while (var != NULL) // searach the list if it is not empty
		{
			if (var->has_value)
			{
				if (vu8_pos(var->has_value) > 0)
				{
					if (vu8_get_offset(var->has_value, 1))
					{
						if (vu8_get_last(var->has_value))
						{
							// values in successive scopes are known
							if (!vvv_compare_last_two(var->values, var->type)) 
							{
								// but the values are different
								if (var->type == T_STRING)
								{
									vvv_free_last_string(var->values);
								}
								// the value will be assigned
								vu8_set_offset(var->has_value, 1, VAR_HAS_CHANGED);	
							}
							vvv_change_last_copy(var->values);
						}
						else
						{
							// value is not known in the closing scope
							vvv_push_copy(&var->values);
							// mark it as changed
							vu8_set_offset(var->has_value, 1, VAR_HAS_CHANGED);
						}
						vu8_set_last(var->has_value, VAR_IS_ASSIGNED);
					}
					else
					{
						if (vu8_get_last(var->has_value))
						{
							if (var->type == T_STRING)
							{
								vvv_free_last_string(var->values);
							}
							vvv_pop(var->values);
						}
						// unset the value of a variable
						vu8_set_last(var->has_value, VAR_HAS_NO_VALUE);
					}
				}
				else
				{
					if (var->type == T_STRING)
					{
						vvv_free_last_string(var->values);
					}
					// discrad the value of a variable that becomes inaccessible
					discard_uint8_vector(&var->has_value);
					discard_var_val_vector(&var->values);
				}
			}
			var = var->next;
			// it is a circular list, stop when the next item was already searched
			if (var == G_var_htab.partial_htabs[G_var_htab.pos][i])
			{
				break;
			}
		}
	}
}

void assign_constants_after_scope()
{
	char *tmp;
	htab_var_t var;
	for (uint8_t i = 0; i < PARTIAL_HT_SIZE; i++)
	{
		var = G_var_htab.partial_htabs[G_var_htab.pos][i];
		while (var != NULL)
		{
			if (var->has_value)
			{
				if (vu8_pos(var->has_value) > 0)
				{
					// if the variable constant value has changed in a scope, it must be assign at the end of it
					if ((vu8_get_last(var->has_value) && !vvv_compare_last_two(var->values, var->type)) || 
					     vu8_get_last(var->has_value) == VAR_HAS_RENEWED_VALUE)
					{
						// insert tokens representing the assign to the token stream
						ltype_insert(T_ADDRESS_ASSGN);
						ltype_insert(T_MOV);
						lvar_insert(var);
						switch(var->type)
						{
							case T_INT:
								lint_insert(vvv_get_last(var->values).integer);
								break;

							case T_BOOL:
								lbool_insert(vvv_get_last(var->values).boolean);
								break;

							case T_STRING:
									tmp = malloc(strlen(vvv_get_last(var->values).string) + 1);
									if (tmp == NULL)
									{
										exit(MEM_ERR);
									}
									strcpy(tmp, vvv_get_last(var->values).string);
									lstring_insert(tmp);
								break;

							case T_FLOAT:
								lfloat_insert(vvv_get_last(var->values).float64);
								break;

							default:
								break;
						}
						ltype_insert(T_ADR_ASSGN_END);

						vu8_set_offset(var->has_value, 1, VAR_HAS_CHANGED);
					}
				}
			}
			var = var->next;
			if (var == G_var_htab.partial_htabs[G_var_htab.pos][i])
			{
				break;
			}
		}
	}
}

void remove_vars_values_in_scope()
{
	htab_var_t var;
	for (uint8_t i = 0; i < PARTIAL_HT_SIZE; i++)
	{
		var = G_var_htab.partial_htabs[G_var_htab.pos][i];
		while (var != NULL)
		{
			if (var->has_value) // if the variable has value (the value can be unknown)
			{
				if (vu8_pos(var->has_value) > 0) // and it has multiple values
				{
					if (vu8_get_last(var->has_value)) // and the value is knwon
					{
						// and the last two values are the same
						if (vvv_pos(var->values) > 0 && vvv_compare_last_two(var->values, var->type))
						{
							vvv_pop(var->values);
							vu8_pop(var->has_value);
							if (vu8_get_last(var->has_value) == VAR_HAS_CHANGED)
							{
								if (var->type == T_STRING)
								{
									vvv_free_last_string(var->values);
								}
								vvv_pop(var->values);
								// but the vaule changed in previous scope, therfore is unknown after the scope
								vu8_set_last(var->has_value, VAR_HAS_NO_VALUE);
							}
						}
						else
						{
							// deallocate string
							if (var->type == T_STRING)
							{
								vvv_free_last_string(var->values);
							}
							// close the current scope
							vvv_pop(var->values);
							vu8_pop(var->has_value);

							if (vu8_get_last(var->has_value))
							{
								 // deallocate string
								if (var->type == T_STRING)
								{
									vvv_free_last_string(var->values);
								}
								vvv_pop(var->values);
								// unset the value in the now current scope
								vu8_set_last(var->has_value, VAR_HAS_NO_VALUE);
							}
						}
					}
					else
					{
						//close the current scope
						vu8_pop(var->has_value);

						if (vu8_get_last(var->has_value))
						{
							if (var->type == T_STRING)
							{
								vvv_free_last_string(var->values);
							}
							// uset the variable value in the now current scope
							vvv_pop(var->values);
						}
						// set the variable value as uknown
						vu8_set_last(var->has_value, VAR_HAS_NO_VALUE);
					}
				}
				else
				{
					if (var->type == T_STRING)
					{
						vvv_free_last_string(var->values);
					}
					// variable becomes inaccessible
					discard_uint8_vector(&var->has_value);
					discard_var_val_vector(&var->values);
				}
			}
			var = var->next;
			if (var == G_var_htab.partial_htabs[G_var_htab.pos][i])
			{
				break;
			}
		}
	}
}

bool has_var_value(htab_var_t var)
{
	return var->has_value && vu8_get_last(var->has_value);
}

uint8_t get_var_state(htab_var_t t)
{
	return t->state;
}

bool get_var_has_value(htab_var_t t)
{
	return t->has_value;
}

int get_var_scope(htab_var_t var)
{
	return var->scope;
}

bool is_special_var(htab_var_t t)
{
	return t == G_var_htab.special_var;
}	

const char *get_var_name(htab_var_t t)
{
	return t->name;
}	

/* ------------------------- "hashtable" - keywords -------------------- */

uint8_t is_key_word(const char *word)
{
	if(word[1] == '\0')
		return NOT_KEYWORD;
	
	switch(word[0])
	{
		case('i'):
			switch(word[1])
			{	
				case('f'):	
					if(word[2] == '\0')
						return IF;
					else
						return NOT_KEYWORD;
				case('n'):
					if(!strcmp("int",word))
						return T_INT;
					else
						return NOT_KEYWORD;
				default:
					return NOT_KEYWORD;
			}
		case('f'):
			switch(word[1])
			{
				case('u'):    				
					if(!strcmp("func", word))
						return FUNC;
					else
						return NOT_KEYWORD;	
				case('o'):
					if(!strcmp("for",word))
						return FOR;
					else
						return NOT_KEYWORD;
				case('a'):
					if(!strcmp("false",word))
						return FALSE;
					else
						return NOT_KEYWORD;
				case('l'):
					if(!strcmp("float64",word))
						return T_FLOAT;
					else
						return NOT_KEYWORD;
				default:
					return NOT_KEYWORD;
			}
		case('s'):
			if(!strcmp("string", word))
				return T_STRING;
			else
				return NOT_KEYWORD;
		case('e'):
			if(!strcmp("else",word))
				return ELSE;
			else
				return NOT_KEYWORD;
		case('p'):
			if(!strcmp("package", word))
				return PACKAGE;
			else
				return NOT_KEYWORD;
		case('t'):
			if(!strcmp("true", word))
				return TRUE;
			else
				return NOT_KEYWORD;
		case('b'):
			if(!strcmp("bool", word))
				return T_BOOL;
			else
				return NOT_KEYWORD;
		case('r'):
			if(!strcmp("return", word))
				return RETURN;
			else
				return NOT_KEYWORD;
		default:
			return NOT_KEYWORD;
	}		
}

uint8_t is_build_in_function(htab_func_t func)
{
	size_t lenght = strlen(func->name);
	if(lenght < 3 || lenght > 9) //the minimum and maximum lenght of build in function names
		return NOT_BUILD_IN;
	switch(func->name[0])
	{
		case('i'):
			if (lenght >= 6)
			{
				switch(func->name[5])
				{
					case('s'):
						if(!strcmp(func->name, "inputs"))
							return T_INS;
						else
							return NOT_BUILD_IN;		
					case('i'):
						if(!strcmp(func->name, "inputi"))
							return T_INI;
						else
							return NOT_BUILD_IN;
					case('b'):
						if(!strcmp(func->name, "inputb"))
							return T_INB;
						else
							return NOT_BUILD_IN;
					case('f'):
						if(!strcmp(func->name, "inputf"))
							return T_INF;
						else
							return NOT_BUILD_IN;
					case('l'):
						if(!strcmp(func->name, "int2float"))
							return T_I2F;
						else
							return NOT_BUILD_IN;
					default:
						return NOT_BUILD_IN;	
				}
			}
			return NOT_BUILD_IN;

		case('p'):
			if(!strcmp(func->name, "print"))
				return T_PRINT;
			else
				return NOT_BUILD_IN;
		case('s'):
			if(!strcmp(func->name, "substr"))
				return T_SUBS;
			else
				return NOT_BUILD_IN;
		case('l'):
			if(!strcmp(func->name, "len"))
				return T_LEN;
			else
				return NOT_BUILD_IN;
		case('o'):
			if(!strcmp(func->name, "ord"))
				return T_ORD;
			else
				return NOT_BUILD_IN;
		case('c'):
			if(!strcmp(func->name, "chr"))
				return T_CHR;
			else
				return NOT_BUILD_IN;
		case('f'):
			if(!strcmp(func->name, "float2int"))
				return T_F2I;
			else
				return NOT_BUILD_IN;
		default:
			return NOT_BUILD_IN;	
	
	}	
}

uint8_t is_function_to_instruction(htab_func_t func)
{
	size_t lenght = strlen(func->name);
	if(lenght < 3 || lenght > 9) //the minimum and maximum lenght of build in function names
		return NOT_BUILD_IN;
	switch(func->name[0])
	{
		case('p'):
			if(!strcmp(func->name, "print"))
				return T_PRINT;
			else
				return NOT_BUILD_IN;

		case('l'):
			if(!strcmp(func->name, "len"))
				return T_LEN;
			else
				return NOT_BUILD_IN;

		case('f'):
			if(!strcmp(func->name, "float2int"))
				return T_F2I;
			else
				return NOT_BUILD_IN;

		case('i'):
			if(!strcmp(func->name, "int2float"))
				return T_I2F;
			else
				return NOT_BUILD_IN;
		default:
			return NOT_BUILD_IN;	
	
	}
}

uint8_t is_input_function(htab_func_t func)
{
	size_t lenght = strlen(func->name);
	if(lenght != 6) //the minimum and maximum lenght of build in function names
		return NOT_BUILD_IN;
	switch(func->name[5])
	{
		case('s'):
			if(!strcmp(func->name, "inputs"))
				return T_INS;
			else
				return NOT_BUILD_IN;		
		case('i'):
			if(!strcmp(func->name, "inputi"))
				return T_INI;
			else
				return NOT_BUILD_IN;
		case('b'):
			if(!strcmp(func->name, "inputb"))
				return T_INB;
			else
				return NOT_BUILD_IN;
		case('f'):
			if(!strcmp(func->name, "inputf"))
				return T_INF;
			else
				return NOT_BUILD_IN;

		default:
			return NOT_BUILD_IN;	
	}
	return NOT_BUILD_IN;
}

/* ---------------- testing functions for printing ---------------- */

void print_all()
{
	printf(" ------------ HASH TABLE VARIABLES START ------------\n");
	T_var_item temp1,temp2;
	for(int64_t j = 0; j < G_var_htab.size; j++ )
	{
		if(G_var_htab.partial_htabs[j])
		{
			printf("--- PARTIAL HTAB NUMBER %ld ---\n", j);	
			for(unsigned i = 0; i < PARTIAL_HT_SIZE; i++)
			{
				if(G_var_htab.partial_htabs[j][i] != NULL)
				{
					printf("FOR INDEX %u ARE STORED ", i);
					for(temp1 = G_var_htab.partial_htabs[j][i], temp2 = temp1 -> next; temp2 != G_var_htab.partial_htabs[j][i];temp1=temp2, temp2=temp2->next)
					{
						printf("(%s and its scope %d)-> ",temp1->name,temp1->scope);
					}
					printf("(%s and its scope %d)\n",temp1->name, temp1->scope);
				}
			}
		}	
	}
	printf(" ------------ HASH TABLE VARIABLES END ------------\n\n");
	
	printf(" ------------ HASH TABLE FUNCTIONS START ------------\n");

	for(unsigned i = 0; i < HT_FUNC_SIZE; i++)
	{
		
		printf("FOR INDEX %u ARE STORED FUNCTIONS ", i);	
		for(T_func_item temp = G_func_htab.hash_table[i]; temp != NULL; temp = temp->next)
		{
			printf("%s -> ",temp->name);
		}
		putchar('\n');		
	}
	printf(" ------------ HASH TABLE FUNCTIONS END ------------\n");

}

char *read_var_name(htab_var_t t)
{
	return t->name;
}

char *read_func_name(htab_func_t f)
{
	return f->name;
}

char *get_var_def_value(htab_var_t var)
{
	switch(var->type)
	{
		case T_INT:
			return "int@0";
		
		case T_FLOAT:
			return "float@0x0p+0";
		
		case T_BOOL:
			return "bool@false";
		
		case T_STRING:
			return "string@";
		
		default:
			return "";
	}
}
