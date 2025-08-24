
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 256
#define MAX_TOKEN_SIZE 16
#define MAX_TOKEN_COUNT (MAX_LINE / MAX_TOKEN_SIZE)
#define MAX_ALIAS_CAPACITY 256
#define MAX_SUB_DIRS 32
#define MAX_FILE_NAME 32
#define MAX_DEPTH 32

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))
#define NO_CLA_PROGRAM() (void)argc; (void)argv;

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
    fprintf(STREAM, "number of defined %s's: %d\n", #EnumName, EnumName##_count);   \
} while(0) 

// error definitions:
// =======================================================================================

#define ERR_LIST(X)                  \
    X(Err_ok)                        \
    X(Err_fgets)                     \
    X(Err_toManyTokens)              \
    X(Err_badArgv)                   \
    X(Err_unkownCMD)                  \
    X(Err_toLittleArgs)               \
    X(Err_aliasMustBeIDF)            \
    X(Err_malloc)                    \
    X(Err_aliasExist)                \
    X(Err_badExitCode)				 \
	X(Err_noDir)					 \
	X(Err_noArgsProvided)			 \
	X(Err_dirExist)					 \
	X(Err_dirTableFull)				 \
	X(Err_dirMustBeIdf)

MAKE_ENUM(Err, ERR_LIST)

const char* error_to_str(int err) {
    const char* res = Err_to_cstr[err];
    if(!res){
        res = "UNKNOWN";
    }
    return res;
}

// shell "data structors":
// =======================================================================================

typedef struct Alias_t {
    char* name;
    int val;
    struct Alias_t* next;
} Alias;

Alias* alias_list = NULL;

typedef struct Dir_entry_t {
	const char* key;
	void* sub_dir;
} Dir_entry;

typedef struct Dir_t
{
	const char* name;
	size_t number_of_sub_dirs;
	struct Dir_t* perent;
	Dir_entry* sub_dirs[MAX_SUB_DIRS];

} Dir;

Dir root_t = { .name = "", .number_of_sub_dirs = 0 };

Dir* root = &root_t;
Dir* current_dir = NULL;

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
Err Program_alias(int argc, char argv[][MAX_TOKEN_SIZE]);
Err Program_dalias(int argc, char argv[][MAX_TOKEN_SIZE]);
Err Program_delate(int argc, char argv[][MAX_TOKEN_SIZE]);

Err Program_cd(int argc, char argv[][MAX_TOKEN_SIZE]);
Err Program_mkdir(int argc, char argv[][MAX_TOKEN_SIZE]);
Err Program_ls(int argc, char argv[][MAX_TOKEN_SIZE]);
Err Program_tree(int argc, char argv[][MAX_TOKEN_SIZE]);

#define CREATE_ENTRY(idf, prog) {.name = idf, .program = prog}

Entry Program_table[] = {
    CREATE_ENTRY("exit", Program_exit),
    CREATE_ENTRY("clear", Program_clear),
    CREATE_ENTRY("dump", Program_dump),
    CREATE_ENTRY("alias", Program_alias),
    CREATE_ENTRY("dalias", Program_dalias),
    CREATE_ENTRY("delate", Program_delate),
	CREATE_ENTRY("cd", Program_cd),
	CREATE_ENTRY("mkdir", Program_mkdir),
	CREATE_ENTRY("ls", Program_ls),
    CREATE_ENTRY("tree", Program_tree),
    
    
    
    CREATE_ENTRY(0, 0),
};

// shell functions:
// =======================================================================================

// printing and error report
void shell_error(int err);
void shell_print_prompt();
void shell_dump_argv(char argv[][MAX_TOKEN_SIZE], int argc);

// parsing and lexing
int shell_split_line(const char* line, char argv[][MAX_TOKEN_SIZE]);
Program shell_getProgram(const char* name);
Err shell_parse_argv(int argc, char argv[][MAX_TOKEN_SIZE]);

// Aliases
void shell_dump_Allalias();
void shell_dump_alias(const char* name);
Alias* shell_getAlias(const char* name);
int shell_AliasSize();
Err shell_add_alias(const char* name, int val);
void shell_remove_alias(const char* name);
void shell_removeALL();

// Dir system
Dir_entry* shell_mkdir(const char* name);
void shell_dumpFile(const Dir* name, FILE* stream);
Dir_entry* shell_getDirEntry(Dir_entry* table[MAX_SUB_DIRS], int table_size, const char* name);
Err shell_chingDir(const char* name);


// =======================================================================================

// implementation:

int main()
{

	current_dir = root;

	char line_buffer[MAX_LINE] = {0};
	char argv[MAX_TOKEN_COUNT][MAX_TOKEN_SIZE];
	int argc = 0;

	while (true) {
	    
		shell_print_prompt();
		if (!fgets(line_buffer, MAX_LINE, stdin)) {
			shell_error(Err_fgets);
		}

		argc = shell_split_line(line_buffer, argv);

		int err = shell_parse_argv(argc, argv);

		shell_error(err);

	}

	return Err_ok;
}

Dir_entry *shell_mkdir(const char *name)
{
    Dir_entry* res = malloc(sizeof(*res));
    if (!res) return NULL;

    Dir* new_dir = malloc(sizeof(*new_dir));
    if (!new_dir) { free(res); return NULL; }

    new_dir->name = strdup(name);
    if (!new_dir->name) { free(new_dir); free(res); return NULL; }

    new_dir->number_of_sub_dirs = 0;
    
    for (int i = 0; i < MAX_SUB_DIRS; i++) new_dir->sub_dirs[i] = NULL;

    res->key = strdup(name);
    if (!res->key) {
        free((char*)new_dir->name); 
        free(new_dir);
        free(res);
        return NULL;
    }
    res->sub_dir = new_dir;

    return res;
}

Dir_entry *shell_getDirEntry(Dir_entry* table[MAX_SUB_DIRS], int table_size, const char *name)
{
    for (int i = 0; i < table_size; i++) {
        Dir_entry* e = table[i];              
        if (e && strcmp(e->key, name) == 0) {
            return e;
        }
    }
    return NULL;
}

Err shell_chingDir(const char* name){
	Dir_entry* entry = shell_getDirEntry(current_dir->sub_dirs,
                                         current_dir->number_of_sub_dirs,
                                         name);
    if (!entry) {
        return Err_noDir;
    }
    current_dir = (Dir*) entry->sub_dir;
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
    printf("in the shell %d aliases:\n", shell_AliasSize());
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

		if (argc >= MAX_TOKEN_COUNT) shell_error(Err_toManyTokens);

	}

	return argc;
}

void shell_dump_argv(char argv[][MAX_TOKEN_SIZE], int argc) {
	for (int i = 0; i < argc; i++) {
		printf("argv[%d] = '%s'\n", i, argv[i]);
	}
}

void shell_print_prompt() {
    printf("Shell ");
	Dir* path[MAX_DEPTH] = {0};
	int i = 0;
	Dir* curr = current_dir;
	while (curr)
	{
		path[i] = curr;
		i++;
		curr = curr->perent;
	}
	for(int k = i - 1; k > 0; k--){
		printf("%s/", path[k]->name);
	}
	printf("%s> ", path[0]->name);
}

void shell_error(int err) {
	if(err == 0) {
		return;
	}
	fprintf(stderr, "ERROR: `%s` (code %d) in shell.\n", error_to_str(err), err);
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
        err = Err_unkownCMD;
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

// programs implementation:
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
	NO_CLA_PROGRAM();
    printf("\033[2J\033[H");
    return Err_ok;
}
Err Program_dump(int argc, char argv[][MAX_TOKEN_SIZE]){
    shell_dump_argv(argv, argc);
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
	NO_CLA_PROGRAM();
    shell_removeALL();
    return Err_ok;
}

Err Program_cd(int argc, char argv[][MAX_TOKEN_SIZE]){
	if (argc < 2) return Err_noArgsProvided;

	if(strcmp(argv[1], "..") == 0 && current_dir != root){
		current_dir = current_dir->perent;
		return Err_ok;
	}
    return shell_chingDir(argv[1]);
}
Err Program_mkdir(int argc, char argv[][MAX_TOKEN_SIZE]){
    if (argc < 2) return Err_noArgsProvided;

    if (current_dir->number_of_sub_dirs >= MAX_SUB_DIRS){
        return Err_dirTableFull; 
	}
	for(size_t i = 0; i < strlen(argv[1]); i++){
		if(!isalpha(argv[1][i]) && !isalnum(argv[1][i]) && argv[1][i] != '_'){
			return Err_dirMustBeIdf;
		}
	}
	if(!isalpha(argv[1][0]) && argv[1][0] != '_'){
		return Err_dirMustBeIdf;
	}

    if (shell_getDirEntry(current_dir->sub_dirs, current_dir->number_of_sub_dirs, argv[1])) {
        return Err_dirExist;
    }

    Dir_entry* e = shell_mkdir(argv[1]);
    if (!e) return Err_malloc;

	Dir* e_sub = e->sub_dir;
	e_sub->perent = current_dir;

    current_dir->sub_dirs[current_dir->number_of_sub_dirs++] = e;
    return Err_ok;
}
Err Program_ls(int argc, char argv[][MAX_TOKEN_SIZE]){
	NO_CLA_PROGRAM();
    for (int i = 0; i < (int)current_dir->number_of_sub_dirs; i++) {
        printf("%s\n", current_dir->sub_dirs[i]->key);
    }
    return Err_ok;
}

void print_tree_aux(Dir* dir, bool *has_sibling, int depth) {
    for (int i = 0; i < depth; i++) {
        if (i == depth - 1) {
            // last depth level: check if this node has siblings
            if (has_sibling[i])
                printf("|---");
            else
                printf("'---");
        } else {
            // parent levels: print vertical bar if parent has more children
            if (has_sibling[i])
                printf("|   ");
            else
                printf("    ");
        }
    }
    printf("%s\n", dir->name);

    for (size_t i = 0; i < dir->number_of_sub_dirs; i++) {
        has_sibling[depth] = (i < dir->number_of_sub_dirs - 1);
        print_tree_aux(dir->sub_dirs[i]->sub_dir, has_sibling, depth + 1);
    }
}

Err Program_tree(int argc, char argv[][MAX_TOKEN_SIZE]) {
    NO_CLA_PROGRAM();
    printf("/\n|");
    bool has_sibling[MAX_DEPTH] = {0};
    print_tree_aux(root, has_sibling, 0);
    return Err_ok;
}

