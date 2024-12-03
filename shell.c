/*Shell*/
/*Lauri Ikonen*/
/*Started 23092024*/
/*Modified 03122024*/

/*Shell with prompt and batch mode,
Built-in commands: cd, path, exit*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>

/*Linked list data structure is used for saving paths and parallel commands*/
typedef struct List {
    char *textData;
    struct List* pNext;
} List;

List * additionList(char *textLine, List *pRoot){
    List *ptr, *pNewNode;
    char *pLine = NULL;

    //Memory allocation for linked list node
    if ((pNewNode = (List*)malloc(sizeof(List))) == NULL){
        char error_message[30] = "Malloc failed\n";
        write(STDERR_FILENO, error_message, strlen(error_message)); 
        exit(1);
    }
    
    //Memory allocation for the line of text
    if ((pLine = (char*)malloc(strlen(textLine)+1)) == NULL){
        char error_message[30] = "Malloc failed\n";
        write(STDERR_FILENO, error_message, strlen(error_message)); 
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
        if (ptr->textData != NULL){
            free(ptr->textData);
        }
        free(ptr);
        ptr = pRoot;
    }
    return pRoot;
};

List * readFile(List *pRoot, char *filename){
    FILE *pFile;
    char *line = NULL;
    size_t len = 0;
    __ssize_t read;

    //file opening with error check
    if ((pFile = fopen(filename, "r")) == NULL){
        char error_message[30] = "Cannot open a file\n";
        write(STDERR_FILENO, error_message, strlen(error_message)); 
        exit(1);
    }

    //read every line from the file
    while ((read = getline(&line, &len, pFile)) != -1 ){
        char fixedSizeLine[len];
        strcpy(fixedSizeLine, line);

        /*first time when adding a List, the root pointer is NULL, thus pointer need to be returned*/
        pRoot = additionList(fixedSizeLine, pRoot);
        
    }
    free(line);
    fclose(pFile);
    return pRoot;
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

    //create list even if no path is given  
    if(pArgument == NULL){
        pPath = additionList("", pPath);
    }

    //list creation
    while(pArgument != NULL){
        pPath = additionList(pArgument, pPath);
        pArgument = strtok(NULL, " ");
    }
    return pPath;
}

int changeDirectory(char * parameter){

    //changing directory with error check 
    if (chdir(parameter) != 0){
        char error_message[40] = "Error occured on built in command cd\n";
        write(STDERR_FILENO, error_message, strlen(error_message)); 
    };
    return 0;
}

int execution(char *command, char *parameter, List *pPath){
    List *ptr;
    char *fullpath_dynamic = NULL;
    char *buffer = NULL;
    char *output = NULL;
    char *shortenedParameter = parameter;


    /*arguments is fixed size and overflow is possible, 
    either need to be dynamic 
    or have a check for out-of-bounds error */
    char *arguments[10]; 
    int i = 1;
    //char *arguments_dynamic;
    //arguments_dynamic = (char*)malloc(sizeof(char)*len(command));    

    //split arguments
    arguments[0]=command;
    if (parameter != NULL){
        if ((output = strstr(parameter, ">")) != NULL){
            shortenedParameter = strtok(parameter, ">");
        }
        buffer = strtok(shortenedParameter, " ");
    }

    while (buffer != NULL){
        arguments[i] = buffer;
        buffer = strtok(NULL, " ");
        i++;
    }
    arguments[i] = NULL;

    pid_t pid = fork(); 
    ptr = pPath;

    //forking failure
    if (pid < 0){
        char error_message[30] = "Fork failed\n";
        write(STDERR_FILENO, error_message, strlen(error_message)); 
        exit(1);
    }

    //successful fork
    if (pid == 0){ 
        
        //redirect shell output

        if(output != NULL){
            char *outputShort;
            int fd;
            outputShort = output + 1;
            
            while(outputShort[0] == ' ' || outputShort[0] == '\t' || outputShort[0] == '\n'){
                outputShort++;
            }
            
            if ((fd = open(outputShort, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) == -1){
                char error_message[35] = "File open error on redirection\n";
                write(STDERR_FILENO, error_message, strlen(error_message)); 
                exit(1);
            }
            
            //changes output to opened file
            dup2(fd, 1);

            close(fd);

        }

        while(ptr != NULL){
            fullpath_dynamic 
            = malloc(sizeof(char) * (2+strlen(ptr->textData)+strlen(command)));
            if(fullpath_dynamic == NULL){
                char error_message[30] = "Malloc failed\n";
                write(STDERR_FILENO, error_message, strlen(error_message)); 
                exit(1);
            }
            fullpath_dynamic[0] = '\0'; //initialized as a empty string
            strcat(fullpath_dynamic, ptr->textData);
            strcat(fullpath_dynamic, "/");
            strcat(fullpath_dynamic, command);
            if (access(fullpath_dynamic, X_OK) == 0){
                execv(fullpath_dynamic,arguments);
                exit(0);
            }

            free(fullpath_dynamic);

            ptr = ptr->pNext;
        }

        //exit from child process if no executable found
        char error_message[30] = "No executable found\n";
        write(STDERR_FILENO, error_message, strlen(error_message)); 
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

        command = strtok(ptr->textData, " ");
        arguments = strtok(NULL, "\n");

        /*Check for builtin commands*/
        if (strcmp("exit", command) == 0){
            pPath = removeList(pPath);
            pCommands = removeList(pCommands);
            exit(0);

        } else if (strcmp("cd", command) == 0){
            arguments = strtok(arguments, " \t\n"); //remove whitespace
            changeDirectory(arguments);
                
        } else if (strcmp("path", command) == 0){
            pPath = updatePath(pPath, arguments);
        
        /*Other commands*/            
        } else {
            pid = fork(); 
            if (pid < 0){
                char error_message[30] = "Fork failed\n";
                write(STDERR_FILENO, error_message, strlen(error_message)); 
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
    while (waitpid(-1, NULL, 0) > 0);


    return 0;
}

List * parseCommands(char *input, List * pCommands){
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

    //Now every command is on linked list with parameters included

    return pCommands;
}

int main(int argc, char *argv[]){
    char *line = NULL;
    char* filename = NULL;
    List *pPath = NULL;
    List *pCommands = NULL;
    List *pBatch = NULL;
    List *ptr = NULL;
    size_t len = 0;

    
    //initiate path    
    pPath = initiatePath(pPath);

    if (argc == 2){
        //fetching commands from batch file
        filename = malloc(sizeof(char)*(strlen(argv[1])+1));
        strcpy(filename, argv[1]);
        pBatch = readFile(pBatch, filename);

        //running batch file commands from linked list 
        ptr = pBatch;
        while (ptr != NULL){
            pCommands = parseCommands(ptr->textData, pCommands);
            processCommands(pPath, pCommands);
            pCommands = removeList(pCommands); //removing executed commands
            ptr = ptr->pNext;
        }
        pBatch = removeList(pBatch);
        pPath = removeList(pPath);

    } else {
        //looping interactive input prompt
        while (1){
            printf("wish>>");
            getline(&line, &len, stdin);
            pCommands = parseCommands(line, pCommands);
            processCommands(pPath, pCommands);
            pCommands = removeList(pCommands); //removing executed commands

        }
        pPath = removeList(pPath);
    }
    return 0;
}
