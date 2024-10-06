/*Shell*/
/*Lauri Ikonen*/
/*Started 23092024*/
/*Modified 01102024*/

/**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(){
    char *line = NULL;
    char *token = NULL;
    size_t len = 0;
    //__ssize_t read;    

    while (1){
        printf("\nshell>>");
        getline(&line, &len, stdin);
        token = strtok(line, " ");
        token = strtok(token, "\n");
        if (strcmp("test", token) == 0){
            printf("test command");
        }

        if (strcmp("exit", token) == 0){
            exit(0);
        }
        
        
    }


    return 0;
}