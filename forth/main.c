
#include "forth.h"

int main(void)
{
    // only memory is static on the main frame(the "process" stack)
    char buffer[MAX_FILE_SIZE] = {0};
    Word words[MAX_WORD_COUNT] = {0};
    Stack stack = {0};
    
    FILE* f = fopen("./tests/test0.forth", "r");
    if(!f){
        return 1;
    }
    fread(buffer, 1, MAX_FILE_SIZE, f);
    fclose(f);
    
    forth_interpret(buffer, words, &stack);

    return 0;
}