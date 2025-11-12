


#include <unistd.h> // for test env

#include <stdint.h>

// fast!
void _uint32_div(uint32_t dividend, uint32_t divisor, uint32_t* q, uint32_t* r){
    if (divisor == 0) {
        *q = UINT32_MAX;
        *r = dividend;
        return;
    }
    uint32_t quotient = 0;
    uint32_t remainder_ = 0;
    for(int i=31; i>=0; i--){
        remainder_ = (remainder_ << 1) | ((dividend >> i) & 1);
        if(remainder_ >= divisor){
            remainder_ -= divisor;
            quotient |= (1U << i);
        }
    }
    *q = quotient;
    *r = remainder_;
}

void _arc0_putc(char c, int dev){
    write(dev, &c, 1); // in the test env...
}

void _arc0_clear(int dev){
    write(dev, "\x1B[2J\x1B[H", 7);  // in the test env...
}

char _arc0_getc(int dev){
    char c;
    read(dev, &c, 1); // in the test env...
    return c;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

char get_char(){
    return _arc0_getc(0);
}

int8_t string_comper(const char* a, const char* b){
    while(*a && *b){
        if(*a < *b) return -1;
        if(*a > *b) return 1;
        a++;
        b++;
    }
    if(*a == *b) return 0;
    if(*a) return 1;   
    return -1;         
}

void serial_clear(){
    _arc0_clear(1);
}

void print_char(char c){
    _arc0_putc(c, 1);
}

void print_string(const char* str){
    while(*str){
        print_char(*str);
        str++;
    }
}

void print_int(uint32_t number, uint8_t sign, uint8_t redix, uint8_t upper){
    if(redix == 0 || redix == 1) redix = 10; // default redix
    
    if(sign && (number >> 31) == 1){
        print_char('-');
        number = ~number + 1;  // 2's complement
    }
    
    if(number == 0){
        print_char('0');
        return;
    }
    
    char buffer[32];
    int i = 0;
    
    while(number > 0){
        // uint32_t r = number % redix;
        // number /= redix;
        uint32_t r = 0;
        uint32_t q = 0;
        _uint32_div(number, redix, &q, &r);
        number = q;
        if(r < 10) buffer[i++] = '0' + r;
        else{
            if(upper == 0) buffer[i++] = 'a' + r - 10;
            else buffer[i++] = 'A' + r - 10;
        } 
    }
    buffer[i] = '\0';
    for(int j = i - 1; j >= 0; j--){
        print_char(buffer[j]);
    }
}

void print_format(const char* fmt, ...)
{
    uint64_t* argp = (uint64_t*)&fmt + 4;  // first arg after fmt
    int argc = 0;
    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            switch (*fmt) {
                case '%':
                    print_char('%');
                    break;
                case 'd':
                    print_int(*argp, 1, 10, 0);
                    argp++;
                    argc++;
                    break;
                case 'u':
                    print_int(*argp, 0, 10, 0);
                    argp++;
                    argc++;
                    break;
                case 'x':
                    print_int(*argp, 1, 16, 0);
                    argp++;
                    argc++;
                    break;
                case 'X':
                    print_int(*argp, 1, 16, 1);
                    argp++;
                    argc++;
                    break;
                case 'b':
                    print_int(*argp, 0, 2, 0);
                    argp++;
                    argc++;
                    break;
                case 's':
                    print_string(*(const char**)argp);
                    argp++;
                    argc++;
                    break;
                case 'c':
                    print_char(*(char*)argp);
                    argp++;
                    argc++;
                    break;
                case 'o':
                    print_int(*argp, 1, 8, 0);
                    argp++;
                    argc++;
                    break;
                case 'p':
                    print_int(*argp, 0, 16, 0);
                    argp++;
                    argc++;
                    break;
                default:
                    print_char(*fmt);
                    break;
            }
            
            // for more then 5 args, for the next stack args vector
            if(argc == 5){
                argp += 18;
            }
            
        } else {
            print_char(*fmt);
        }
        fmt++;
    }
}

void test0(){
    // BASIC INTEGER TESTS
    print_string("=== BASIC INTEGER TESTS ===\n");
    print_format("%d\n", 0);
    print_format("%d\n", 123);
    print_format("%d\n", -123);
    print_format("%u\n", 123);
    print_format("%u\n", (1u << 31));

    // HEX TESTS
    print_string("\n=== HEX TESTS ===\n");
    print_format("%x\n", 0x0);
    print_format("%x\n", 0x1234);
    print_format("%x\n", 0xabcdef);
    print_format("%X\n", 0xabcdef);
    print_format("%X\n", 0xABCDEF);

    // BINARY TESTS
    print_string("\n=== BINARY TESTS ===\n");
    print_format("%b\n", 0b0);
    print_format("%b\n", 0b1010);
    print_format("%b\n", 0xA5);

    // OCTAL TESTS
    print_string("\n=== OCTAL TESTS ===\n");
    print_format("%o\n", 0);
    print_format("%o\n", 0123);
    print_format("%o\n", 0347);
    
    // CHAR TESTS
    print_string("\n=== CHAR TESTS ===\n");
    print_format("%c\n", 'H');
    print_format("%c  and %c\n", 'a', 'Y');
    print_format("%c,  %c, %c \n", 'f', 'F', 'M');

    // STRING TESTS
    print_string("\n=== STRING TESTS ===\n");
    print_format("%s\n", "Hello");
    print_format("%s %s %s\n", "from", "the", "va_fanc");

    // POINTER TESTS
    print_string("\n=== POINTER TESTS ===\n");
    int x;
    print_format("%p %p\n", &x, (void*)0x1234ABCD);

    // COMBINED TEST
    print_string("\n=== COMBINED TEST ===\n");
    print_format("dec=%d hex=%x bin=%b str=%s char=%c\n",
                 -255, 0xABCD, 0b10101010, "this is a literal string.", 'Z');
    print_format("dec=%d hex=%x HEX=%X bin=%b oct=%o str=%s char=%c unsigned=%u ptr=%p\n",
                 -255, 0xABCD, 0xABCD, 0b10101010, 0347, "test", 'Z', (1u << 31), &x);

    // EDGE CASES
    print_string("\n=== EDGE CASES ===\n");
    print_format("%d\n", -2147483648);
    print_format("%u\n", 4294967295u);
    print_format("literal %% sign\n");
    print_format("%x %x %x\n", 0, 0xF, 0x10);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define MAX_LINE        32
#define MAX_ARGV        16
#define MAX_ARG_SIZE    32
#define MAX_FS_DEPTH    16

typedef enum {
    ATTR_NON = 0,
    ATTR_ROOT,
    ATTR_TXT,
    ATTR_EXE,
    ATTR_DIR,
} ATTR;

typedef struct File_t {
    char* name;
    uint8_t attr;
    void* data;
    struct File_t* perent;
} File_t;

typedef uint8_t (*Code)(uint8_t argc, char argv[MAX_ARGV][MAX_ARG_SIZE]);

File_t file_root = {.name = "", .attr = ATTR_ROOT, .perent = NULL};
File_t* file_system = &file_root;
File_t* pwd = &file_root;

File_t* File_find_aux(const char* path, File_t* curr) {
    if (!path || !*path)
        return curr; // reached end of path

    uint8_t i = 0;
    char tmp[32];

    // Copy next component
    while (path[i] && path[i] != '/' && i < sizeof(tmp) - 1){
        tmp[i] = path[i];
        i++;
    }

    tmp[i] = '\0';

    File_t** list = (File_t**)curr->data;

    for (uint8_t j = 0; list[j] != NULL; j++) {
        if (string_comper(list[j]->name, tmp) == 0) {
            if (path[i] == '/')
                return File_find_aux(path + i + 1, list[j]);
            else
                return list[j];
        }
    }

    return NULL; // not found
}
File_t* File_find(const char* path){
    if(!path) return NULL;
    
    uint8_t i = 0;
    File_t* curr = NULL;
    if(path[0] == '/'){
        return File_find_aux(path+1, &file_root);
        
    }
    return File_find_aux(path, pwd);
}

uint8_t sh_code(uint8_t argc, char argv[MAX_ARGV][MAX_ARG_SIZE]);
uint8_t te_code(uint8_t argc, char argv[MAX_ARGV][MAX_ARG_SIZE]){}
uint8_t as_code(uint8_t argc, char argv[MAX_ARGV][MAX_ARG_SIZE]){}
uint8_t ls_code(uint8_t argc, char  argv[MAX_ARGV][MAX_ARG_SIZE]){
    File_t* file = NULL;
    if(argc >= 2){
        file = File_find(argv[1]);
    }
    else{
        file = pwd;
    }
    File_t** list = (File_t**)file->data;
    int i = 0;
    while (list[i] != NULL) {
        print_char('\t');
        print_string(list[i]->name);
        print_char('\n');
        i++;
    }
    return 0;
}
uint8_t cd_code(uint8_t argc, char  argv[MAX_ARGV][MAX_ARG_SIZE]){
    if(argc < 2){
        print_string("ERROR: no path was given.\n");
        return 1;
    }
    if(string_comper(argv[1], "..") == 0){
        pwd = pwd->perent;
        return 0;
    }
    File_t* file = File_find(argv[1]);
    if(!file){
        print_format("ERROR: file: `%s` is not in a sub dir of the current dir.\n", argv[1]);
        return 1;
    }
    if(file->attr != ATTR_DIR && file->attr != ATTR_ROOT){
        print_format("ERROR: file: `%s` is not a dir.\n", argv[1]);
        return 1;
    }
    pwd = file;
    return 0;
}
uint8_t clear_code(uint8_t argc, char  argv[MAX_ARGV][MAX_ARG_SIZE]){
    serial_clear();
    return 0;
}
uint8_t help_code(uint8_t argc, char argv[MAX_ARGV][MAX_ARG_SIZE]) {
    print_string("\n");
    print_string("*********************************************\n");
    print_string("***           ARCH0 SHELL HELP            ***\n");
    print_string("*********************************************\n");
    print_string("***  Built-in Commands:                   ***\n");
    print_string("***                                       ***\n");
    print_string("***  help    - Show this help message     ***\n");
    print_string("***  ls      - List directory contents    ***\n");
    print_string("***  cd      - Change directory           ***\n");
    print_string("***  cat     - Display text file contents ***\n");
    print_string("***  clear   - Clear the screen           ***\n");
    print_string("***  dump    - Show given arguments       ***\n");
    print_string("***  exit    - Log out / terminate shell  ***\n");
    print_string("***                                       ***\n");
    print_string("***  Executable files (.exe) can be run   ***\n");
    print_string("***  directly by name or with full path.  ***\n");
    print_string("***                                       ***\n");
    print_string("***  Examples:                            ***\n");
    print_string("***    ~$ hello.exe                       ***\n");
    print_string("***    ~$ /bin/cat hello.txt              ***\n");
    print_string("***                                       ***\n");
    print_string("***  Notes:                               ***\n");
    print_string("***   - Paths can be absolute or relative ***\n");
    print_string("***   - Use '..' to go up a directory     ***\n");
    print_string("***   - 'clear' resets the terminal       ***\n");
    print_string("***   - Arguments are space-separated     ***\n");
    print_string("*********************************************\n\n");
    return 0;
}
uint8_t cat_code(uint8_t argc, char  argv[MAX_ARGV][MAX_ARG_SIZE]){
    if(argc < 2){
        print_string("ERROR: no file was given.\n");
        return 1;
    }
    const char* data = NULL;
    File_t* file = File_find(argv[1]);
    if(!file){
        print_format("ERROR: file: `%s` is not in a sub dir of the current dir.\n", argv[1]);
        return 1;
    }
    if(file->attr != ATTR_TXT){
        print_format("ERROR: file: `%s` is not a TXT file.\n", argv[1]);
        return 1;
    }
    print_string((const char*)file->data);
    return 0;
}

uint8_t hello_code(uint8_t argc, char argv[MAX_ARGV][MAX_ARG_SIZE]){
    print_format("Hello world!\n");
    return 0;
}
uint8_t dump_code(uint8_t argc, char  argv[MAX_ARGV][MAX_ARG_SIZE]){
    print_format("\nnumber of args: %d\n", argc);
    print_string("==================\n");
    for(int i = 0; i < argc; i++){
        print_format("[%d] = %s\n", i, argv[i]);
    }
    print_string("==================\n\n");
    return 0;
}

File_t file_sh          = {.name = "sh",    .attr = ATTR_EXE, .data = sh_code};
File_t file_te          = {.name = "te",    .attr = ATTR_EXE, .data = te_code};
File_t file_as          = {.name = "as",    .attr = ATTR_EXE, .data = as_code};
File_t file_ls          = {.name = "ls",    .attr = ATTR_EXE, .data = ls_code};
File_t file_cd          = {.name = "cd",    .attr = ATTR_EXE, .data = cd_code};
File_t file_clear       = {.name = "clear", .attr = ATTR_EXE, .data = clear_code};
File_t file_help        = {.name = "help",  .attr = ATTR_EXE, .data = help_code};
File_t file_cat         = {.name = "cat",   .attr = ATTR_EXE, .data = cat_code};

File_t file_hello_txt   = {.name = "hello.txt", .attr = ATTR_TXT, .data = "#deinfe <stdio.h>\n\nint main()\n{\n\tprint_string(\"Hello World!\");\n\treturn 0;\n}\n"};
File_t file_hello_exe   = {.name = "hello.exe", .attr = ATTR_EXE, .data = hello_code};
File_t file_bob_txt     = {.name = "bob.txt",   .attr = ATTR_TXT, .data = "i am bob!\n"};
File_t file_bin         = {.name = "bin",       .attr = ATTR_DIR, .perent = &file_root};
File_t file_home        = {.name = "home",      .attr = ATTR_DIR, .perent = &file_root};
File_t file_tmp         = {.name = "tmp",       .attr = ATTR_DIR, .perent = &file_root};
File_t file_etc         = {.name = "etc",       .attr = ATTR_DIR, .perent = &file_root};
File_t file_ailce       = {.name = "alice",     .attr = ATTR_DIR, .perent = &file_home};
File_t file_bob         = {.name = "bob",       .attr = ATTR_DIR, .perent = &file_home};
File_t file_dump        = {.name = "dump",      .attr = ATTR_EXE, .data = dump_code};

File_t* bin_data[]      = { &file_sh, &file_te, &file_as, &file_ls, &file_cd, &file_clear, &file_help, &file_cat, NULL};
File_t* alice_data[]    = { &file_hello_txt, &file_hello_exe, NULL};
File_t* bob_data[]      = { &file_bob_txt, NULL};
File_t* home_data[]     = { &file_ailce, &file_bob, NULL};
File_t* tmp_data[]      = { &file_dump, NULL};
File_t* etc_data[]      = { NULL};
File_t* root_data[]     = { &file_bin, &file_home, &file_tmp, &file_etc, NULL};

void init_fs(){
    
    file_root.data  = root_data;
    file_bin.data   = bin_data;
    file_home.data  = home_data;
    file_tmp.data   = tmp_data;
    file_etc.data   = etc_data;
    file_ailce.data = alice_data;
    file_bob.data   = bob_data;
    
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

uint8_t execute(uint8_t argc, char argv[MAX_ARGV][MAX_ARG_SIZE]){
    if(argc < 1){
        print_string("ERROR: no EXE was given.\n");
        return 1;
    }
    File_t* file = File_find(argv[0]);
    if(!file){
        char tmp[MAX_LINE];
        uint8_t i = 0;
        tmp[i++] = '/';
        tmp[i++] = 'b';
        tmp[i++] = 'i';
        tmp[i++] = 'n';
        tmp[i++] = '/';
        for (uint8_t j = 0; argv[0][j] != '\0' && i < MAX_LINE - 1; j++, i++){
            tmp[i] = argv[0][j];
        }
        tmp[i] = '\0';
        file = File_find(tmp);
    }
    if(!file){
        print_format("ERROR: file: `%s` is not in the system.\n", argv[0]);
        return 1;   
    }
    if(file->attr != ATTR_EXE){
        print_format("ERROR: file: `%s` is not a EXE file.\n", argv[0]);
        return 1;   
    }
    Code prog = (Code)file->data;
    return prog(argc, argv);
}

void wellcom(){
    print_string("\n");
    print_string("*********************************************\n");
    print_string("***        [ CORE SYSTEM ONLINE ]          ***\n");
    print_string("***        arch0 Shell v0.1 initialized    ***\n");
    print_string("***     Enter 'help' to access commands    ***\n");
    print_string("*********************************************\n\n");
}

void prompt()
{
    char* path[MAX_FS_DEPTH] = {0};
    File_t* curr = pwd;
    uint8_t i = 0;
    while(curr){
        path[i] = curr->name;
        i++;
        curr = curr->perent;
    }
    for(int j = i - 1; j >= 0; j--){
        print_string(path[j]);
        print_char('/');
    }
    print_string("~$ ");
}

void get_line(char line[MAX_LINE])
{
    uint8_t i = 0;
    char c;

    while (1) {
        c = get_char();

        if (c == '\n' || c == '\r') {
            break;
        }
        else if ((c == '\b' || c == 127) && i > 0) {  // backspace or DEL
            i--;                       
            print_string("\b \b");     
        }
        else if (i < MAX_LINE - 1 && c >= 32 && c <= 126) { 
            line[i++] = c;
            // echo in the real env...
        }
    }

    line[i] = '\0';
}

uint8_t parese_line(char line[MAX_LINE], char argv[MAX_ARGV][MAX_ARG_SIZE])
{
    uint8_t argc = 0;
    uint8_t i = 0; // index in current argument

    for (uint8_t j = 0; line[j] != '\0'; j++) {
        char c = line[j];

        if (c == ' ' || c == '\t') {  
            if (i > 0) {
                argv[argc][i] = '\0';
                argc++;
                i = 0;
                if (argc >= MAX_ARGV) break;
            }
        } else {
            if (i < MAX_ARG_SIZE - 1) {
                argv[argc][i++] = c;
            }
        }
    }

    if (i > 0 && argc < MAX_ARGV) {
        argv[argc][i] = '\0';
        argc++;
    }

    return argc;
}

uint8_t shell(){
    char line[MAX_LINE];
    char argv[MAX_ARGV][MAX_ARG_SIZE];
    uint8_t err;
    
    wellcom();
    
    while(1){
        err = 0;
        prompt();
        get_line(line);
        uint8_t argc = parese_line(line, argv);
        if(argc == 0) continue;
        
        if(string_comper("exit", argv[0]) == 0){
            print_string("exeting shell...\n");
            break;
        }
        else{
            err = execute(argc, argv);
        }
    }
    return 0;
}

int main()
{
    init_fs();
    uint8_t err = shell();
    print_string("log out...\n"); // last "prosses" in system is done sow loging out from user, and reset system
    return err;
}

uint8_t sh_code(uint8_t argc, char argv[MAX_ARGV][MAX_ARG_SIZE]){
    shell();
    return 0;
}

