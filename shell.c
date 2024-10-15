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


int main(){
    char *line = NULL;
    char *token = NULL;
    char *parameter = NULL;
    size_t len = 0;
    //__ssize_t read;    

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
                exit(1);
            };
            continue;
        }

        if (strcmp("path", token) == 0){
            printf("built-in path\n");
            if (chdir(parameter) != 0){
                perror("path failed!");
                exit(1);
            };
            continue;
        }


        if (strcmp("exit", token) == 0){
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
- redirection
- paths
- parallel commands (&)

- change if-else structure for switch case

should i be editing path at ~/.bashrc

*/