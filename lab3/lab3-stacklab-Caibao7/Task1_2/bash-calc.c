#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#define MAXLEN 15

extern int gets(char *buf);

const char wrapper_front[] = "echo $((";
const char wrapper_back[] = "))";

char command[MAXLEN+16];

void eval(char *cmd) {
    printf("Executing: %s\n", cmd);
    system(cmd);
    fflush(stdout);
    return;
}

char* input(char *cmd) {
    char expression[MAXLEN+1];
    printf("Enter an expression (length up to %d):\n", MAXLEN);
    if(!gets(expression) || strlen(expression) == 0) {
        printf("No expression.\n");
        exit(0);
    }
    int len = strlen(expression) > MAXLEN ? MAXLEN : strlen(expression);
    strcpy(cmd, wrapper_front);
    strcpy(cmd+strlen(wrapper_front)+len, wrapper_back);
    strncpy(cmd+strlen(wrapper_front), expression, len);
}

int main() {
    while(1){
        input(command);
        eval(command);
    }
    return 0;
}
