
#include "forth.h"

void forth_interpret(const char* src, Word words[MAX_WORD_COUNT], Stack* stack){
    uint16_t word_count = forth_lex(src, words);
    forth_parse(words, word_count);
    forth_evaluate(words, word_count, stack);
}

uint16_t forth_lex(const char* src, Word words[MAX_WORD_COUNT]){
    int wi = 0; // word index
    const char* p = src;

    while(*p && wi < MAX_WORD_COUNT){
        // skip whitespace
        while(*p == ' ' || *p == '\t' || *p == '\n') p++;
        
        // skip one line comments
        if(*p && *p == '\\'){
            p++;
            while(*p && *p != '\n') p++;
            if(!*p) forth_error("comment not closed.");
            p++;
        }
        
        // skip () comments
        if(*p && *p == '('){
            p++;
            while(*p && *p != ')') p++;
            if(!*p) forth_error("comment not closed.");
            p++;
        }

        if(!*p) break;

        // start of token
        const char* start = p;
        while(*p && *p != ' ' && *p != '\t' && *p != '\n') p++;

        uint8_t len = p - start;
        if(len == 0) continue;

        words[wi].name = start;
        words[wi].size = len;

        wi++;
    }
    
    return wi;
}

void forth_parse(Word words[MAX_WORD_COUNT], Cell word_count){
    forth_dump_words(words, word_count);
}

void forth_evaluate(Word words[MAX_WORD_COUNT], Cell word_count, Stack* stack){
    
}

void forth_dump_words(Word words[MAX_WORD_COUNT], Cell word_count){
    printf("number of words: %u\n", word_count);
    for(Cell i = 0; i < word_count; i++){
        Word_dump(&words[i]);
    }
}

void Word_dump(Word* w){
    for(Cell i = 0; i < w->size; i++){
        putchar(w->name[i]);
    }
    putchar('\n');
}

void forth_error(const char* msg){
    printf("ERROR: %s\n", msg);
    exit(1);
}

void Stack_push(Stack* s, Cell item){
    if(s->sp >= MAX_STACK_SIZE - 1) forth_error("stack over flow.");
    s->data[s->sp] = item;
    s->sp++;
}

Cell Stack_pop(Stack* s){
    if(s->sp == 0) forth_error("stack under flow.");
    s->sp--;
    return s->data[s->sp];
}
