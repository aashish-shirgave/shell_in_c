#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>


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

cmd *redirect_command(cmd *subcmd, char *filename, int type);

cmd *execute_command();

cmd *parse_for_redirects(cmd *ret, char **start, char *end);

char *string_copy(char *start, char *end);


void run_command(cmd *command);
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
        fprintf(stderr, "The unexecuted commands %s\n", command);
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

/**/
cmd *parse_to_execute(char **start, char *end){
    char *q, *eq;
    int token, argc;
    execmd *command;
    cmd *ret;

    ret = execute_command();

    command = (execmd*) ret;
    argc = 0;

    ret = parse_for_redirects(ret, start, end);
    while(!peek(start, end, "|")){
        if((token = get_token(start, end, &q, &eq)) == 0){
            break;
        }
        if(token == 'a'){
            fprintf(stderr, "syntax error in the command\n");
            exit(-1);
        }
        command ->argv[argc] == string_copy(q, eq);
        argc ++;
        if(argc > MAXARGS){
            fprintf(stderr, "Too many arguments to the command\n");
            exit(-1);
        }
        ret = parse_for_redirects(ret, start, end);

    }
    command->argv[argc] = 0;

}

/**/
cmd *execute_command(){
    execmd *command;
    command = malloc(sizeof(*command));
    memset(command, 0, sizeof(command));
    command->type = ' ';
    return (cmd*) command;
}


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

/**/
cmd *parse_for_redirects(cmd *ret, char **start, char *end){
    int tokens;
    char *q, *eq;

    while(peek(start, end, "><")){
        tokens = get_token(start, end, 0, 0);
        if(get_token(start, end, &q, &eq) == 'a'){
            fprintf(stderr, "missing file fpr redirection ");
            exit(-1);
        }
        switch (tokens){
            case '<' :
                ret = redirect_command(ret, string_copy(q, eq), '<');
                break;
            
            case '>' :
                ret = redirect_command(ret, string_copy(q, eq), '>');
                break;
        }
    }
    return ret;
}

/**/
char *string_copy(char *start,  char *end){
    int num;
    num = end - start;
    char *ret;
    ret = (char *)malloc(num);
    if(ret == NULL){
        fprintf(stderr, "Could not malloc");
        exit(-1);
    }
    strncpy(ret, start, num);
    ret[num] = '\0';
    return ret;
}

/***/
cmd *redirect_command(cmd *subcmd, char *filename, int type){
    redircmd *command;
    command = malloc(sizeof(*command));
    memset(command, 0, sizeof(*command));

    command->type = type;
    command->cmd = subcmd;
    command->filename = filename;
    command->mode = (type == '<') ? O_RDONLY : O_WRONLY|O_CREAT|O_TRUNC;
    command->fd = (type == '<') ? 0 : 1;
    return (cmd *) command;
}

void run_command(cmd *command){
    int pfd[2], i;
    execmd *exec_command;
    pipecmd *pipe_command;
    redircmd *redir_command;

    switch(command->type){
        default :
            fprintf(stderr, "command not found");
            exit(-1);

        case ' ' :
            exec_command = (execmd*)command;
            char path1[100] = "/bin/";
            char path2[100] = "/bin/usr/";
            
            if(exec_command->argv[0] == 0){
                exit(0);
            }
            execv(exec_command->argv[0], exec_command->argv);
            execv(strcat(path1,exec_command->argv[0]),exec_command->argv);
            execv(strcat(path2,exec_command->argv[0]),exec_command->argv);
            fprintf(stderr, "Execution failed or command not found - %s\n", exec_command->argv[0]);
            break;

        case '>' :
        case '<' :
            
            redir_command = (redircmd *) command;
            close(redir_command->fd);
            if(open(redir_command -> filename, redir_command->mode) < 0){
                fprintf(stderr, "File cannot be opened - %s\n", redir_command->filename);
            }
            run_command(redir_command->cmd);
            break;
        
        case '|' :
            pipe_command = (pipecmd*)command;
            if(pipe(pfd) < 0){
                fprintf(stderr, "Pipe error\n");
                exit(-1);
            }
            if(fork() == 0){
                close(1);
                dup(pfd[1]);
                close(pfd[0]);
                close(pfd[1]);
                run_command(pipe_command->left);
            }
            if(fork() == 0){
                close(0);
                dup(pfd[0]);
                close(pfd[0]);
                close(pfd[1]);
                run_command(pipe_command->right);
            }
            close(pfd[0]);
            close(pfd[1]);
            wait(0);
            wait(0);
            break;

    }
    exit(0);

}

int main(){
    print_prompt();
    char *command;
    int r, pid;

    do{
        printf("aashish");
        command = readline(prompt);
        printf("The command is %s \n", command);
        //checking for command cd
        if(check_cd(command)){
            continue;
        }
        
        pid = fork();
        if(pid == -1){
            perror("fork failed");
        }
        if(pid == 0){
            run_command(parse_command(command));
        }
        wait(&r);
        
        //run_command(command);

        free(command);
    }while(1);
    
}
