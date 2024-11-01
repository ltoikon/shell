/*Shell*/
/*Lauri Ikonen*/
/*Started 23092024*/
/*Modified 29102024*/

/**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/*Linked list data structure is used for saving paths*/
//Can I use same struct for saving parallel commands
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
    //char fullpath[1000] = {0};
    char *fullpath_dynamic = NULL;
    char *buffer = NULL;
    //char *buffer_dynamic = NULL;

    /*arguments is fixed size and overflow is possible, 
    either need to be dynamic 
    or have a check for out-of-bounds error */
    char *arguments[10]; 
    //char *arguments_dynamic;
    int i = 1;
    
    //arguments_dynamic = (char*)malloc(sizeof(char)*len(command));
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

    pid_t pid = fork(); 
    ptr = pPath;

    if (pid < 0){
        perror("Fork failed!");
        exit(1);
    }

    if (pid == 0){
        while(ptr != NULL){
            fullpath_dynamic 
            = malloc(sizeof(char) * (2+strlen(ptr->textData)+strlen(command)));
            strcat(fullpath_dynamic, ptr->textData);
            strcat(fullpath_dynamic, "/");
            strcat(fullpath_dynamic, command);
            //printf("TEST command and path is: %s\n", fullpath);
            if (access(fullpath_dynamic, X_OK) == 0){
                execv(fullpath_dynamic,arguments);
            }
            //clear fullpath string for a next path at the list
            //memset(fullpath_dynamic,0,sizeof(fullpath_dynamic)); //static
            //memset(fullpath,0,strlen(fullpath)); //dynamic

            free(fullpath_dynamic);

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


int processCommands(List * pPath, List * pCommands){

    char *command;
    char *arguments;
    pid_t pid;
    List *ptr = pCommands;
    
    //Next these commands should be executed parallel
    //Splitting parameters is done inside each execute function

    while (ptr != NULL){

        fprintf(stdout, "Executing: ");
        fprintf(stdout, "%s\n", ptr->textData);

        command = strtok(ptr->textData, " ");
        arguments = strtok(NULL, "\n");

        /*Check for builtin commands*/

        if (strcmp("exit", command) == 0){
            pPath = removeList(pPath);
            pCommands = removeList(pCommands);
            exit(0);

        } else if (strcmp("vaihto", command) == 0){
                arguments = strtok(arguments, " \t\n"); //remove whitespace
                changeDirectory(arguments);
                
        } else if (strcmp("path", command) == 0){
                    pPath = updatePath(pPath, arguments);
                    printList(pPath);
        
        /*Other commands*/            
        } else {
            pid = fork(); 
            if (pid < 0){
                perror("Fork failed!");
                exit(1);
            } else if (pid == 0){

                if (pPath != NULL && strcmp(pPath->textData, "") != 0){
                    execution(command, arguments, pPath);
                } else {
                    printf("No paths given\n");
                }

                exit(0); //Stopping child process
            }
            
        } 
        ptr = ptr->pNext;
    }        
    
    /*Wait for child processes before returning
    https://stackoverflow.com/questions/279729/how-to-wait-until-all-child-processes-called-by-fork-complete*/
    while (wait(NULL) > 0);

    return 0;
}

List * parseCommands(char *input, List * pPath, List * pCommands){
    
    
    //List *pCommands = NULL;
    char *command = NULL;

    //removing linebreak only if string is not empty
    if (strcmp("\n", input) != 0){
        input = strtok(input, "\n");
    }

    command = strtok(input, "&");

    while(command != NULL){
        pCommands = additionList(command, pCommands);
        command = strtok(NULL, "&");
    }

    //TODO: TRIM whitespace https://stackoverflow.com/questions/122616/how-do-i-trim-leading-trailing-whitespace-in-a-standard-way

    //Now every command is on linked list with parameters included

    return pCommands;
}

int main(){
    char *line = NULL;
    List *pPath = NULL;
    List *pCommands = NULL;
    size_t len = 0;

    //initiate path    
    pPath = initiatePath(pPath);
    //printList(pPath);

    //looping interactive input prompt
    while (1){
        printf("shell>>");
        getline(&line, &len, stdin); //not freed,  does it leak memory?
        pCommands = parseCommands(line, pPath, pCommands);
        processCommands(pPath, pCommands);
        pCommands = removeList(pCommands); //remove executed commands

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
- parallel commands (&) kindof

TODO:
- redirection (>)
- batch file handling

POSSIBLE CHANGES:
- change linked list for dynamic list, does it matter? which one is better?
    -use realloc for dynamic list when it grows
    -earlier implementation used it, did change for linked list
- change if-else structure for switch case
- some places do-while loop would be easier to read
EXTRA:
- command history would be cool
    -readline

should i have pipe | ?
*/