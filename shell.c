/*Shell*/
/*Lauri Ikonen*/
/*Started 23092024*/
/*Modified 14102024*/

/**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

char * initiatePath(char *path){
    char *defaultPath = "/bin/";

    //allocate memory
    path = malloc(sizeof(*path) * strlen(defaultPath));
    if (path == NULL){
        perror("malloc failed!");
    }

    strcpy(path, defaultPath);
    printf("%s\n",path);
    return path;
}

char * updatePath(char *path, char *addition){
    char *newPath;
    size_t newSize = strlen(path) + strlen(addition) + 2;
    printf("in update function, next reallocation\n");
    //memory reallocation
    newPath = realloc(path, newSize);
    if (newPath == NULL){
        perror("realloc failed!");
    }
    printf("reallocation is done\n");
    newPath = strncat(newPath,":",2);
    newPath = strncat(newPath,addition,strlen(addition));
    printf("%s\n",newPath);

    return newPath;
}

int main(){
    char *line = NULL;
    char *token = NULL;
    char *parameter = NULL;
    char *path = NULL;
    size_t len = 0;
    //__ssize_t read;

    //initiate path    
    path = initiatePath(path);
    printf("%s\n",path);
    while (1){
        
        printf("shell>>");
        getline(&line, &len, stdin);
        token = strtok(line, " ");
        parameter = strtok(NULL, " ");
        parameter = strtok(parameter, "\n"); //remove line break from parameter
        printf("command is %s, and given parameter if given: %s\n", token, parameter);

        /*remove linebreak only if string is not empty*/
        if (strcmp("\n", token) != 0){
            token = strtok(token, "\n");
        }

        //no extra forks if no command 
        if (strcmp("\n", token) == 0){
            continue;
        }

        if (strcmp("vaihto", token) == 0){
            printf("built-in cd\n");
            if (chdir(parameter) != 0){
                perror("cd failed!");
            };
            continue;
        }

        if (strcmp("path", token) == 0){
            printf("built-in path\n");

            if (parameter != NULL) {
                path = updatePath(path, parameter);
            } else {
                printf("No parameter provided for path update\n");
            }
            //path = updatePath(path, parameter);
            continue;
        }


        if (strcmp("exit", token) == 0){
            free(path);
            path = NULL;
            exit(0);
        } else {
            pid_t pid = fork();
            if (pid < 0){
                perror("Fork failed!");
                exit(1);
            }
            if (pid == 0){
                char *args[]={token, parameter, NULL};
                execvp(args[0],args);
            } else {
                wait(NULL);
            }
        }
        
    }


    return 0;
}

/*fork() - creates new process, child is copy of the parent
exec() -
wait() -

TODO:
- built-in commands 
    o exit
    o cd
    - path
- redirection (>)
- paths
- parallel commands (&)

- change if-else structure for switch case

should i have pipe | ?
should i be editing file ~/.bashrc?


To parse the input line into constituent pieces, you might want to use strsep()
*/