#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

#define MAXARGS 16
/****** Global Variables *************/
static char prompt[512];
char cwd[1024];
char *command;


/*************************************/
/****** Structures *******************/
typedef struct command{
    int type;
}cmd;

typedef struct command_to_execute_by_shell {
    int type;
    char *cmd;
    char *argv[MAXARGS];
}execmd;

typedef struct redirection_command {
    int type;
    cmd *cmd;
    char *filename;
    int mode;
    int fd;
}redircmd;

typedef struct pipe_command{
    int type;
    cmd *left;
    cmd *right;
}pipecmd;




/*************************************/
/****** All Functions ****************/
void print_prompt();
//void tokanize_with_pipe();




/*************************************/
void print_prompt(){
    if(getcwd(cwd, sizeof(cwd)) != NULL){
        strcpy(prompt,"My prompt :");
        strcat(prompt, cwd);
        strcat(prompt, " $");
        //printf("%s", prompt);
    }
    else{
        perror("Error in getting Current working directory");
        exit(1);
    }
    
}

int main(){
    print_prompt();
    //char *command;
    do{
        command = readline(prompt);
        printf("The command is \"%s\"\n", command);
        

        free(command);
    }while(1);
    
}
