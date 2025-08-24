
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 256
#define MAX_TOKEN_SIZE 16
#define MAX_TOKEN_COUNT (MAX_LINE / MAX_TOKEN_SIZE)
#define DEFAULT_PROMPT "6502-shell>"
#define MAX_ALIAS_CAPASITY 256

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

// macro magic:
// =======================================================================================

#define ENUM_ITEM(name) name,
#define CSTR_ITEM(name) #name,

#define MAKE_ENUM(EnumName, LIST)                   \
    typedef enum {                                  \
        LIST(ENUM_ITEM)                             \
        EnumName##_count                            \
    } EnumName;                                     \
                                                    \
    const char* EnumName##_to_cstr[] = {            \
        LIST(CSTR_ITEM)                             \
    };
    
#define DUMP_ENUM(EnumName, STREAM) do {                                            \
    for(int i = 0; i  < EnumName##_count; i++){                                     \
        fprintf(STREAM, "number: %d, is: `%s`\n", i, EnumName##_to_cstr[i]);        \
    }                                                                               \
    fprintf(STREAM, "namber of defined %s's: %d\n", #EnumName, EnumName##_count);   \
} while(0) 

// error defintions:
// =======================================================================================

#define ERR_LIST(X)                  \
    X(Err_ok)                        \
    X(Err_fgets)                     \
    X(Err_toManyTokens)              \
    X(Err_badArgv)                   \
    X(Err_unkonCMD)                  \
    X(Err_toLitelArgs)               \
    X(Err_aliasMustBeIDF)            \
    X(Err_malloc)                    \
    X(Err_aliasExist)                \
    X(Err_badExitCode)

MAKE_ENUM(Err, ERR_LIST)

const char* error_to_str(int err) {
    const char* res = Err_to_cstr[err];
    if(!res){
        res = "UNKWON";
    }
    return res;
}

// shell "deta structors":
// =======================================================================================

const char* PROMPT = DEFAULT_PROMPT;

typedef struct Alias_t {
    char* name;
    int val;
    struct Alias_t* next;
} Alias;

Alias* alias_list = NULL;

typedef Err (*Program)(int argc, char argv[][MAX_TOKEN_SIZE]);

typedef struct Entry_t {
    const char* name;
    Program program;
} Entry;

// programs table:
// =======================================================================================

Err Program_exit(int argc, char argv[][MAX_TOKEN_SIZE]);
Err Program_clear(int argc, char argv[][MAX_TOKEN_SIZE]);
Err Program_dump(int argc, char argv[][MAX_TOKEN_SIZE]);
Err Program_prompt(int argc, char argv[][MAX_TOKEN_SIZE]);
Err Program_alias(int argc, char argv[][MAX_TOKEN_SIZE]);
Err Program_dalias(int argc, char argv[][MAX_TOKEN_SIZE]);
Err Program_delate(int argc, char argv[][MAX_TOKEN_SIZE]);

#define CREATE_ENTRY(idf, prog) {.name = idf, .program = prog}

Entry Program_table[] = {
    CREATE_ENTRY("exit", Program_exit),
    CREATE_ENTRY("clear", Program_clear),
    CREATE_ENTRY("dump", Program_dump),
    CREATE_ENTRY("prompt", Program_prompt),
    CREATE_ENTRY("alias", Program_alias),
    CREATE_ENTRY("dalias", Program_dalias),
    CREATE_ENTRY("delate", Program_delate),
    
    
    
    CREATE_ENTRY(0, 0),
};

// shell functions:
// =======================================================================================

// printing and error report
void shell_error(int err);
void shell_print_prompt(const char* prompt);
void shell_dump_argv(char argv[][MAX_TOKEN_SIZE], int argc);

// parsing and lexing
int shell_split_line(const char* line, char argv[][MAX_TOKEN_SIZE]);
Program shell_getProgram(const char* name);
Err shell_parse_argv(int argc, char argv[][MAX_TOKEN_SIZE]);

// Aliass
Err shell_getAliasIndex(const char* name);
void shell_dump_Allalias();
void shell_dump_alias(const char* name);
Alias* shell_getAlias(const char* name);
int shell_AliasSize();
Err shell_add_alias(const char* name, int val);
void shell_remove_alias(const char* name);
void shell_removeALL();

// =======================================================================================

// implamantion:

int main()
{
	char line_buffer[MAX_LINE] = {0};
	char argv[MAX_TOKEN_COUNT][MAX_TOKEN_SIZE];
	int argc = 0;

	while (true) {
	    
		shell_print_prompt(PROMPT);
		if (!fgets(line_buffer, MAX_LINE, stdin)) {
			shell_error(Err_fgets);
		}

		argc = shell_split_line(line_buffer, argv);

		int err = shell_parse_argv(argc, argv);

		shell_error(err);

	}

	return Err_ok;
}

void shell_removeALL(){
    int size = shell_AliasSize();
    while(size > 0){
        Alias* prev = NULL;
        Alias* curr = alias_list;
        while(curr){
            prev = curr;
            curr = curr->next; 
        }
        shell_remove_alias(prev->name);
        size--;
    }
}

void shell_remove_alias(const char* name) {
    Alias* prev = NULL;
    Alias* curr = alias_list;

    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) {
                prev->next = curr->next;
            } else {
                alias_list = curr->next;
            }
            free(curr->name);
            free(curr);
        }
        prev = curr;
        curr = curr->next;
    }
}

Err shell_add_alias(const char* name, int val){
    if(!isalpha(name[0]) && name[0] != '_'){
        return Err_aliasMustBeIDF;
    }
    Alias* tmp = shell_getAlias(name);
    if(tmp){
        tmp->val = val;
        return Err_ok;
    }
    
    Alias* new_alias = (Alias*)malloc(sizeof(*new_alias));
    if(!new_alias){
        return Err_malloc;
    }
    new_alias->name = strdup(name);
    new_alias->val = val;
    if(!alias_list){
        alias_list = new_alias;
        return Err_ok;
    } 
    
    Alias* prev = NULL;
    Alias* curr = alias_list;
    while(curr){
        prev = curr;
        curr = curr->next;
    }
    
    prev->next = new_alias;
    
    return Err_ok;
}

void shell_dump_alias(const char* name){
    Alias* alias = shell_getAlias(name);
    if(!alias){
        printf("alias: `%s` dont exist\n", name);
        return;
    }
    printf("alias: `%s`, val: %d\n", alias->name, alias->val);
}

void shell_dump_Allalias(){
    Alias* curr = alias_list;
    if(!curr){
        printf("no alias in the shell\n");
        return;
    }
    printf("in the shell %d aliass:\n", shell_AliasSize());
    while(curr){
        shell_dump_alias(curr->name);
        curr = curr->next;
    }
}

int shell_AliasSize(){
    int len = 0;
    Alias* curr = alias_list;
    while(curr){
        len++;
        curr = curr->next;
    }
    return len;
}

Alias* shell_getAlias(const char* name){
    Alias* curr = alias_list;
    while(curr){
        if(strcmp(curr->name, name) == 0){
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

int shell_split_line(const char* line, char argv[][MAX_TOKEN_SIZE]) {
	const char* curr = line;
	int i = 0;
	int argc = 0;

	while(*curr) {

		// trim
		while (*curr == ' ' || *curr == '\t' || *curr == '\n') {
			curr++;
		}
		if (*curr == '\0') break;

		// token
		int i = 0;
		while (*curr && *curr != ' ' && *curr != '\t' && *curr != '\n' && i < MAX_TOKEN_SIZE - 1) {
			argv[argc][i++] = *curr++;
		}
		argv[argc][i] = '\0';
		argc++;

		if (argc >= MAX_TOKEN_COUNT) shell_error(2);

	}

	return argc;
}

void shell_dump_argv(char argv[][MAX_TOKEN_SIZE], int argc) {
	for (int i = 0; i < argc; i++) {
		printf("argv[%d] = '%s'\n", i, argv[i]);
	}
}

void shell_print_prompt(const char* prompt) {
	printf("%s ", prompt);
}

void shell_error(int err) {
	if(err == 0) {
		return;
	}
	fprintf(stderr, "ERROR: `%s`, code: 0x%.2x in shell.\n", error_to_str(err), err);
}

Err shell_parse_argv(int argc, char argv[][MAX_TOKEN_SIZE]) {
    if(argc == 0){
        return Err_ok;
    }
	if(!argv) {
		return Err_badArgv;
	}
	const char* cmd = argv[0];

    Err err = Err_ok;
    Program program = shell_getProgram(cmd);
    if(program){
        err = program(argc, argv);
    }
    else{
        err = Err_unkonCMD;
    }
    return err;
}

Program shell_getProgram(const char* name){
    int i = 0;
    while(Program_table[i].name){
        if(strcmp(Program_table[i].name, name) == 0){
            return Program_table[i].program;
        }
        i++;
    }
    return NULL;
}

// programs implemantion:
//=============================================================================

Err Program_exit(int argc, char argv[][MAX_TOKEN_SIZE]){
    int code = 0;
	if(argc > 1){
	    if(!isdigit(argv[1][0])){
	        code = Err_badExitCode;
	    } 
	    else{
	        code = atoi(argv[1]);
	    } 
	}
	printf("exiting shell...\n");
	shell_removeALL();
	exit(code);
}

Err Program_clear(int argc, char argv[][MAX_TOKEN_SIZE]){
    system("clear");
    return Err_ok;
}
Err Program_dump(int argc, char argv[][MAX_TOKEN_SIZE]){
    shell_dump_argv(argv, argc);
    return Err_ok;
}
Err Program_prompt(int argc, char argv[][MAX_TOKEN_SIZE]){
    if(argc < 2){
	   PROMPT = DEFAULT_PROMPT;
	   return Err_ok;
	}
	PROMPT = argv[1];
	return Err_ok;
}
Err Program_alias(int argc, char argv[][MAX_TOKEN_SIZE]){
    if(argc == 1){
	    shell_dump_Allalias();
	}
	else if(argc == 2){
	    shell_dump_alias(argv[1]);
	}
	if(argc == 3){
	    return shell_add_alias(argv[1], atoi(argv[2]));
	}
	return Err_ok;
}
Err Program_dalias(int argc, char argv[][MAX_TOKEN_SIZE]){
    for(int i = 1; i < argc; i++){
        shell_remove_alias(argv[i]);
    }
    return Err_ok;
}
Err Program_delate(int argc, char argv[][MAX_TOKEN_SIZE]){
    shell_removeALL();
    return Err_ok;
}
