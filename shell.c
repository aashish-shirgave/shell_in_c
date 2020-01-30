#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

#define MAXARGS 16
/****** Global Variables *************/
char space[] = " \t\r\n\v";
static char *prompt;
char cwd[1024];
char symbols[] = "<>|";

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
//For printing the prompt of the shell
void print_prompt();

//For checking if the command is cd to change directory
int check_cd(char * command);

//For parsing command to the data structure
cmd *parse_command(char *command);

//The pipe divides the commands means pipe differentiate the commands
cmd *parse_with_pipe(char **start, char *end);

cmd *parse_to_execute(char **start, char *end_ptr);

int peek(char **start, char *end_ptr, char *tokens);


int get_token(char **start,  char *end, char **q,  char **eq);

cmd *pipe_command(cmd *left, cmd *right);
cmd *execute_command();


//void run_command();
//cmd execute_command();




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

/**/
int check_cd(char * command){
    if(command[0] == 'c' && command[1] == 'd' && command[2] == ' '){
        
        if(chdir(command + 3) < 0 ){
            fprintf(stderr, "*%s* Directory not found\n", command + 3);
        }
        if(getcwd(cwd, sizeof(cwd)) != NULL){
            strcpy(prompt,"My prompt :");
            strcat(prompt, cwd);
            strcat(prompt, " $");
        }
    }
    else{
        return 0;
    }
    return 1;
}

/**/
cmd *parse_command(char * command){
    char *end_ptr;
    cmd * cmdd;
    end_ptr = command + strlen(command);

    cmdd = parse_with_pipe(&command, end_ptr);

    peek(&command, end_ptr, "");

    if(command != end_ptr){
        fprint(stderr, "The unexecuted commands %s\n", command);
        exit(-1);
    }
    return cmdd;

}

/**/
cmd *parse_with_pipe(char **start, char *end){
    cmd *command;
    command = parse_to_execute(start, end);

    if(peek(start, end, "|")){
        get_token(start, end, 0, 0);
        command = pipe_command(command, parse_with_pipe(start, end));
    }
    return command;
}

cmd *parse_to_execute(char **start, char *end){
    char *q, *eq;
    int token, argc;
    execmd *command;
    cmd *ret;

    ret = execute_command();

}

cmd *exec


/**/
int peek(char **start, char *end_ptr, char *tokens){
    char *temp;

    temp = *start;
    while(temp < end_ptr && strchr(space, *temp)){
        temp++;
    }
    *start = temp;
    return *temp && strchr(tokens, *temp);
}

/**/
int get_token(char **start, char *end, char **q, char **eq){

    char *temp;
    int ret;
    temp = *start;
    while(temp < end && strchr(space, *temp)){
        temp++;
    }
    if(q != NULL){
        *q = temp;
    }
    ret = *temp;

    switch(*temp){
        case 0 :
            break;
        case '|' :
        case '<' :
        case '>' :
            temp++;
            break;

        default :
            ret = 'a';
            while(temp < end && !strchr(space, *temp) && !strchr(symbols, *temp)){
                temp++;
            }
            break;
    }
    if(eq != 0){
        *eq = temp;
    }
    while(temp < end && strchr(space, *temp)){
        temp ++;
    }
    return ret;
}

/**/
cmd *pipe_command(cmd *left, cmd *right){
    pipecmd *command;

    command = malloc(sizeof(*command));
    memset(command, 0, sizeof(*command));

    command->type = '|';
    command->left = left;
    command->right = right;

    return (cmd*)command;
}



int main(){
    print_prompt();
    char *command;
    do{
        command = readline(prompt);
        printf("The command is \"%s\"\n", command);
        //checking for command cd
        if(check_cd(command)){
            continue;
        }
        //run_command(command);

        free(command);
    }while(1);
    
}
