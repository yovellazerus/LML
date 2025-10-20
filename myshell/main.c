#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_SIZE 256
#define MAX_ARGS 32

int main(void) {
    char line[MAX_LINE_SIZE];

    while (1) {
        printf("$ ");
        fflush(stdout);

        if (fgets(line, sizeof(line), stdin) == NULL)
            break; // EOF or error

        // remove newline
        char *nl = strchr(line, '\n');
        if (nl) *nl = '\0';

        if (line[0] == '\0')
            continue;

        // exit built-in
        if (strcmp(line, "exit") == 0)
            break;

        // split line into args
        char *argv[MAX_ARGS];
        int argc = 0;
        char *token = strtok(line, " ");
        while (token && argc < MAX_ARGS - 1) {
            argv[argc++] = token;
            token = strtok(NULL, " ");
        }
        argv[argc] = NULL;

        pid_t pid = fork();

        if (pid < 0) {
            perror("fork failed");
            continue;
        }
        else if (pid == 0) {
            // child
            execvp(argv[0], argv);
            perror("exec failed");
            exit(1);
        }
        else {
            // parent
            int status;
            waitpid(pid, &status, 0);
        }
    }

    printf("Bye!\n");
    return 0;
}
