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

/*Linked list used for saving paths*/
typedef struct List {
    char *textData;
    struct List* pNext;
} List;

List * additionList(char *textLine, List *pRoot){
    List *ptr, *pNewNode;
    char *pLine = NULL;

    //Memory allocation for linked list node
    if ((pNewNode = (List*)malloc(sizeof(List))) == NULL){
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }
    
    //Memory allocation for the line of text
    if ((pLine = (char*)malloc(strlen(textLine)+1)) == NULL){
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }

    //Values for new List
    if (textLine == NULL){
        strcpy(textLine, "empty, lineread error");
    }
    strcpy(pLine, textLine);

    pNewNode->textData = pLine;
    pNewNode->pNext = NULL;

    //Add new list node to linked list structure
    //Initialize linked list if empty
    if (pRoot == NULL){
        pRoot = pNewNode;
    }

    //Linked list existing already
    else {
        ptr = pRoot;
        int i=0;
        while (ptr->pNext != NULL){
            ptr = ptr->pNext;
            i++;
        }
        ptr->pNext = pNewNode;
    }
    return pRoot;
}

List * removeList(List *pRoot){
    List *ptr = pRoot;
    //Delete linked list node by node
    while (ptr != NULL){
        pRoot = ptr->pNext;
        free(ptr->textData);
        free(ptr);
        ptr = pRoot;
    }
    return pRoot;
};

int writeList(List *pRoot, char *filename){
    List *ptr = pRoot;

    if (strcmp("stdout", filename) != 0){
        FILE *pFile;
        if ((pFile = fopen(filename, "w")) == NULL){
            fprintf(stderr, "error: cannot open file '%s'\n", filename);
            exit(1);
        }
        while (ptr != NULL){
            fprintf(pFile, "%s", ptr->textData);
            ptr = ptr->pNext;
        }
        fclose(pFile);
    }
    else {
        //printf("Output:\n");
        while (ptr != NULL){
        fprintf(stdout, "%s", ptr->textData);
        ptr = ptr->pNext;
        }
    }
    return 0;
}



List * initiatePath(List *pPath){
    char *defaultPath = "/bin";
    pPath = additionList(defaultPath, pPath);

    return pPath;


}
/*TODO: 
 - new path replaces old paths, so need to edit this
 - should i changes how path is saved to array*/

List * updatePath(List *pPath, char *addition){
    /*
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
    */
    // remove old path
    
    removeList(pPath);


    return pPath;
}

int main(){
    char *line = NULL;
    char *token = NULL;
    char *parameter = NULL;
    List *pPath = NULL;
    //int argumentsCount = NULL;
    size_t len = 0;
    //__ssize_t read;

    //initiate path    
    pPath = initiatePath(pPath);
    //printf("%s\n",path);
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
                updatePath(path, parameter);
            } else {
                printf("No parameter provided for path update\n");
            }
            //path = updatePath(path, parameter);
            continue;
        }


        if (strcmp("exit", token) == 0){
            free(path);
            //path[] = NULL; //How to make sure that path pointer is made to 
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