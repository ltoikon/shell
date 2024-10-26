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

/*Linked list data structure is used for saving paths*/
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

int printList(List *pRoot){
    List *ptr = pRoot;

    printf("Paths:\n");
    while (ptr != NULL){
        fprintf(stdout, "%s\n", ptr->textData);
        ptr = ptr->pNext;
    }
    
    return 0;
}



List * initiatePath(List *pPath){
    char *defaultPath = "/bin";
    pPath = additionList(defaultPath, pPath);

    return pPath;


}

List * updatePath(List *pPath, char *addition){
    char * pArgument = strtok(addition, " ");
  
    // remove old path
    pPath = removeList(pPath);
    
    while(pArgument != NULL){
        printf("%s\n", pArgument);
        pPath = additionList(pArgument, pPath);
        pArgument = strtok(NULL, " ");
    }
    

    return pPath;
}
int changeDirectory(char * parameter){ 
    //printf("built-in cd\n");
    if (chdir(parameter) != 0){
        perror("cd failed!");
    };
    return 0;
}

int execution(char *command, char *parameter, List *pPath){
    List *ptr;
    char fullpath[1000] = {0};
    char *buffer = NULL;
    char *arguments[10]; //remove fixed size, overflow possible
    int i = 1;
 
    //split arguments
    arguments[0]=command;
    if (parameter != NULL){
        buffer = strtok(parameter, " ");
    }

    while (buffer != NULL){
        arguments[i] = buffer;
        buffer = strtok(NULL, " ");
        i++;
    }
    arguments[i] = NULL;

    //TODO
        // käy läpi jokainen path etsien accessilla commandia
        // kun löytyy aja command
        // jos ei löydy pathista ei ajoa
    pid_t pid = fork();
    ptr = pPath;

    if (pid < 0){
        perror("Fork failed!");
        exit(1);
    }

    if (pid == 0){
        while(ptr != NULL){
            strcat(fullpath, ptr->textData);
            strcat(fullpath, "/");
            strcat(fullpath, command);
            //printf("TEST command and path is: %s\n", fullpath);
            if (access(fullpath, X_OK) == 0){
                execv(fullpath,arguments);
            }
            //clear fullpath string for a next path at the list
            memset(fullpath,0,sizeof(fullpath)); //static
            //memset(fullpath,0,strlen(fullpath)); //dynamic

            // found on https://stackoverflow.com/questions/8107826/proper-way-to-empty-a-c-string

            ptr = ptr->pNext;
        }
        //exit from child process if no executable found
        printf("No executable found\n");
        exit(1);
    } else {
        wait(NULL);
    }
    
    

    return 0;
}

int main(){
    char *line = NULL;
    char *token = NULL;
    char *parameter = NULL;
    List *pPath = NULL;
    size_t len = 0;
    //__ssize_t read;

    //initiate path    
    pPath = initiatePath(pPath);
    printList(pPath);
    while (1){
        
        printf("shell>>");
        getline(&line, &len, stdin); //line not any point freed, does it leak memory?
        token = strtok(line, " ");
        parameter = strtok(NULL, "\n");
        //parameter = strtok(parameter, "\n"); //remove line break from parameter
        //printf("command is %s, and given parameter if given: %s\n", token, parameter);

        /*remove linebreak only if string is not empty*/
        if (strcmp("\n", token) != 0){
            token = strtok(token, "\n");
        }

        //no extra forks if no command 
        if (strcmp("\n", token) == 0){
            continue;
        }

        if (strcmp("vaihto", token) == 0){
            changeDirectory(parameter);
            continue;
        }

        if (strcmp("path", token) == 0){
            //printf("built-in path\n");
            pPath = updatePath(pPath, parameter);
            printList(pPath);
            continue;
        }


        if (strcmp("exit", token) == 0){
            pPath = removeList(pPath);
            //path[] = NULL; //How to make sure that path pointer is made to 
            exit(0);
        } else {
            //printf("outside executioner\n");
            //printf("token: %s, parameter: %s, first path on list: %s \n",token, parameter, pPath->textData);
            
            //
            if (pPath != NULL && strcmp(pPath->textData, "") != 0){
                execution(token, parameter, pPath);
            
            } else {
                printf("No paths given\n");
            }
        }
    }

    return 0;
}

/*fork() - creates new process, child is copy of the parent
exec() -
wait() -

DONE:
 built-in commands 
    o exit
    o cd
    o path

TODO:
- redirection (>)
- parallel commands (&)
- batch file handling

POSSIBLE CHANGES:
- change linked list for dynamic list, does it matter? which one is better?
    -use realloc for dynamic list when it grows
    -earlier implementation used it, did change for linked list
- change if-else structure for switch case

EXTRA:
- command history would be cool
    -readline

should i have pipe | ?


To parse the input line into constituent pieces, you might want to use strsep()
another function could be strtok_r, not sure is it better
-strtok is thread safe https://www.geeksforgeeks.org/strtok-strtok_r-functions-c-examples/
*/
