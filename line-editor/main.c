
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// =============================================================================
// data structers and defines:
// =============================================================================

#define MAX_LINE_SIZE   (32*32)
#define MAX_TOKEN_SIZE  32
#define MAX_TOKEN_COUNT (MAX_LINE_SIZE / MAX_TOKEN_SIZE)
#define MAX_FILE_SIZE   (32*32*8)

typedef struct Node{
    char*   data;
    int     number;
    struct  Node* next; 
    struct  Node* prev; 
} Node;

typedef struct List{
    Node* head;
    Node* tail;
    int size;
    Node* pc;   // "program counter"
} List;

Node*   Node_create(int number, char* data);
void    Node_destroy(Node* node);

List*   List_create();
void    List_destroy(List* list);
int     List_getSize(List* list);
void    List_insert(List* list, int number, char* data);
void    List_dump(List* list);
void    List_to_cstr(List* list, char* buff);
void    List_from_FILE(List* list, FILE* file);

Node* Node_create(int number, char* data){
    Node* res = malloc(sizeof(Node));
    if(!res){
        return NULL;
    }
    
    res->data = strdup(data);
    res->number = number;
    res->next = NULL;
    res->prev = NULL;
    return res;
}

void  Node_destroy(Node* node){
    if (!node) return;
    free(node->data);
    free(node);
}

List*   List_create(){
    List* res = malloc(sizeof(List));
    if(!res){
        return NULL;
    }
    
    res->head = Node_create(0, "\n");
    if(!res->head){
        free(res);
    }
    res->tail = Node_create(-1, "\n");
    if(!res->tail){
        free(res->head);
        free(res);
    }
    
    res->pc = res->head;
    
    res->head->next = res->tail;
    res->tail->prev = res->head;
    res->size = 0;
    return res;
}

void    List_destroy(List* list){
    if(!list) return;
    Node* curr = list->head;
    while (curr) {
        Node* next = curr->next;
        Node_destroy(curr);
        curr = next;
    }
    free(list);
}

void List_insert(List* list, int number, char* data) {
    if (!list || !data) return;

    Node* curr = list->head->next;

    while (curr != list->tail) {
        // Overwrite existing line
        if (curr->number == number) {
            free(curr->data);
            curr->data = strdup(data);
            return;
        }

        if (curr->number > number) break;

        curr = curr->next;
    }

    Node* new_node = Node_create(number, data);
    if (!new_node) return;

    new_node->next = curr;
    new_node->prev = curr->prev;
    curr->prev->next = new_node;
    curr->prev = new_node;

    list->size++;
}

int     List_getSize(List* list){
    if(!list) return 0;
    return list->size;
}

void    List_dump(List* list){
    if(!list){
        return;
    }
    Node* curr = list->head;
    while(curr != list->tail){
        printf("%s", curr->data);
        curr = curr->next;
    }
    printf(curr->data);
}

void    List_to_cstr(List* list, char* buff){
    if(!list || !buff){
        return;
    }
    Node* curr = list->head;
    while(curr != list->tail){
        snprintf(buff + strlen(buff), MAX_FILE_SIZE - strlen(buff), "%s", curr->data);
        curr = curr->next;
    }
}

// =============================================================================
// haders for the editor:
// =============================================================================

typedef enum {
  OK = 0,
  NOPROG,
  SEGFULT,
  KERNEL,
  NOTOK,
  NOLANGINST,
  
  UNKNOWN,
} Err;

char line_buffer[MAX_LINE_SIZE];
char argv[MAX_TOKEN_COUNT][MAX_TOKEN_SIZE];
int argc;
List* list = NULL;
char program_as_cstr[MAX_FILE_SIZE];

void error(Err err);
void init();
void prompt();
void getLine();
int  parseLine();
void exeLine();
void addLine(int number);
bool saveFile(char* file_name);
bool loadFile(char* file_name);

Err     List_run(List* list);
bool    List_end(List* list);
Err     List_excute_line(List* list);
Err     List_exeToken(List* list, const char* tok);
Err     List_nextLine(List* list);

// =============================================================================
// editor implemention:
// =============================================================================

int main()
{
    init();
    
    while(1){
        prompt();
        getLine();
        parseLine();
        exeLine();
    }

    printf("\033[0;32m");
    return 1;
}

void error(Err err){
    switch (err){
        case OK:
            printf("\nOK.\n");
            break;
        case NOPROG:
            printf("ERROR: no user program.\n");
            break;
        case SEGFULT:
            printf("ERROR: SEGFULT.\n");
            break;
        case KERNEL:
            printf("ERROR: kernel error.\n");
            break;
        case NOTOK:
            printf("ERROR: no token error.\n");
            break;
        case NOLANGINST:
            printf("ERROR: not a valide instraction in the langwich.\n");
            break;
        default:
            printf("ERROR: UNKNOWN error.\n");
            break;
    }
}

// the function that do the havy basic-like syntax 
Err     List_exeToken(List* list, const char* tok){
    if(!list){
        return NOPROG;
    }
    if(!tok){
        return NOTOK;
    }
    
    // if(strcmp(tok, "print") == 0){
        
    // }
    // else if(strcmp(tok, "goto") == 0){
        
    // }
    
    
    
    // else{
    //     return NOLANGINST;
    // }
    
    // demo
    printf("%s, ", tok);
    
    return OK;
}

Err    List_nextLine(List* list){
    list->pc = list->pc->next;
    if(list->pc == NULL) return SEGFULT;
    return OK;
}

bool    List_end(List* list){
    if(!list) return NOPROG;
    if(list->pc == list->tail){
        return true;
    }
    return false;
}

Err List_excute_line(List* list){
    
    if(!list) return NOPROG;
    Err err = OK;
    memcpy(line_buffer, list->pc->data, MAX_LINE_SIZE);
    argc = parseLine();
    
    for(int i = 0; i < argc; i++){
        err = List_exeToken(list, argv[i]);
        if(err != OK) return err;
    }
    
    return err;
}

Err List_run(List* list){
    if(!list){
        return NOPROG;
    }
    Err err = OK;
    
    list->pc = list->head->next;
    while(!List_end(list)){
        err = List_excute_line(list);
        if(err != OK) return err;
        err = List_nextLine(list);
        if(err != OK) return err;
    }
    
    return OK;
}

void init(){
    printf("\033[1;32m");
    system("cls");
    list = List_create();
    if(!list){
        return;
    }
    printf("**** Hello to the 6502 user line editor interface! ****\n");
}

void prompt(){
    printf("> ");
}

void getLine(){
    fgets(line_buffer, MAX_LINE_SIZE, stdin);
}

int parseLine()
{
    argc = 0;
    char tok[MAX_TOKEN_SIZE];
    char *str = line_buffer;

    // main parser loop:
    while (*str && *str != '\n') {
        
        // trim white space
        while (*str == ' ' || *str == '\t') str++;

        // string literal
        if (*str == '"') {
            str++; 
            int i = 0;
            while (*str && *str != '"' && i < MAX_TOKEN_SIZE - 1) {
                tok[i] = *str;
                i++;
                str++;
            }
            tok[i] = '\0';
            if (*str == '"') str++;
            
        } 
        // all aver token types...
        else {
            int i = 0;
            while (*str && *str != ' ' && *str != '\n' && i < MAX_TOKEN_SIZE - 1) {
                tok[i] = *str;
                i++;
                str++;
            }
            tok[i] = '\0';
        }

        if (tok[0] == '\0') break;
        memcpy(argv[argc], tok, MAX_TOKEN_SIZE);
        argc++;
    }

    return argc;
}

void exeLine(){
    
    if(argc == 0) return;
    
    int number = atoi(argv[0]);
    if(number != 0){
        addLine(number);
        return;
    }
    
    // cmd for the editor:
    if(strcmp(argv[0], "exit") == 0){
        printf("exiting...\n\033[0;0m");
        List_destroy(list);
        exit(0);
    }
    else if(strcmp(argv[0], "list") == 0){
        List_dump(list);
    }
    else if(strcmp(argv[0], "clear") == 0){
        system("cls");
    }
    else if(strcmp(argv[0], "new") == 0){
        List_destroy(list);
        list = List_create();
    }
    else if(strcmp(argv[0], "save") == 0){
        if(argc < 2){
            printf("ERROR: no file name was provided.\n");
        }
        if(argc > 2){
            printf("ERROR: to many args.\n");
        }
        printf("saving...\n");
        if(!saveFile(argv[1])) printf("ERROR: can not open file: `%s`.\n", argv[1]);
        else printf("done!\n");
    }
    else if(strcmp(argv[0], "load") == 0){
        if(argc < 2){
            printf("ERROR: no file name was provided.\n");
        }
        if(argc > 2){
            printf("ERROR: to many args.\n");
        }
        printf("loading...\n");
        if(!loadFile(argv[1])) printf("ERROR: can not load file: `%s`.\n", argv[1]);
        else printf("done!\n");
    }
    else if(strcmp(argv[0], "run") == 0){
        error(List_run(list));
    }
    
    
    else{
        printf("ERRPR: no editor cammend: `%s`.\n", argv[0]);
    }
}

void addLine(int number){
    List_insert(list, number, line_buffer);
}

bool saveFile(char* file_name){
    FILE* f = fopen(file_name, "w");
    if(!f){
        return false;
    }
    List_to_cstr(list, program_as_cstr);
    fwrite(program_as_cstr, 1, strlen(program_as_cstr), f);
    memset(program_as_cstr, 0, MAX_FILE_SIZE);
    fclose(f);
    return true;
}

bool loadFile(char* file_name){
    FILE* f = fopen(file_name, "r");
    if(!f){
        return false;
    }
    List_from_FILE(list, f);
    fclose(f);
    return true;
}

void    List_from_FILE(List* list, FILE* file){
    if(!list || !file){
        return;
    }
    while(fgets(program_as_cstr, MAX_FILE_SIZE, file)){
        if(program_as_cstr[0] == '\n') continue;
        char* end;
        double hack = strtod(program_as_cstr, &end);
        List_insert(list, (int)hack, program_as_cstr);
        memset(program_as_cstr, 0, MAX_FILE_SIZE);
    }
}
