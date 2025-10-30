#ifndef FORTH_H_
#define FORTH_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_FILE_SIZE   (256 * 32)
#define MAX_WORD_COUNT  (256 * 2 )
#define MAX_STACK_SIZE  (256 * 8 )
#define MAX_DICT_SIZE   (256 * 2 )
#define MAX_WORD_SIZE   (MAX_FILE_SIZE / MAX_WORD_COUNT)

typedef uint32_t Cell;
typedef uint8_t  Byte;

typedef struct Word {
    const char* name;
    uint8_t size;              // name size
    struct Word* patch;     // code for user words
} Word;

void Word_dump(Word* w);

typedef struct Stack {
    Cell data[MAX_STACK_SIZE];
    uint16_t sp;
} Stack;

void Stack_push(Stack* s, Cell item);
Cell Stack_pop(Stack* s);

void forth_interpret(const char* src, Word words[MAX_WORD_COUNT], Stack* stack);
uint16_t forth_lex(const char* src, Word words[MAX_WORD_COUNT]);
void forth_parse(Word words[MAX_WORD_COUNT], Cell word_count);
void forth_evaluate(Word words[MAX_WORD_COUNT], Cell word_count, Stack* stack);
void forth_dump_words(Word words[MAX_WORD_COUNT], Cell word_count);
void forth_error(const char* msg);

#endif  // FORTH_H_