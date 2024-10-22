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

int execution(char * command, char * argument, List *pPath){

    //TODO
        // käy läpi jokainen path etsien accessilla commandia
        // kun löytyy aja command
        // jos ei löydy pathista ei ajoa
    pid_t pid = fork();
    if (pid < 0){
        perror("Fork failed!");
        exit(1);
    }
    if (pid == 0){
        char *args[]={command, argument, NULL};
        execvp(args[0],args);
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
        getline(&line, &len, stdin);
        token = strtok(line, " ");
        parameter = strtok(NULL, "\n");
        //parameter = strtok(parameter, "\n"); //remove line break from parameter
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
            execution(token, parameter, pPath);
            
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