#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

/****** Global Variables *************/
static char prompt[512];
char cwd[1024];
char *command;


/*************************************/

/****** All Functions ****************/
void print_prompt();
void tokanize_with_pipe();




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
void tokanize_with_pipe(){
    
}

int main(){
    print_prompt();
    //char *command;
    do{
        command = readline(prompt);
        printf("%s\n", command);
        tokenize_with_pipe();

        free(command);
    }while(1);
    
}
