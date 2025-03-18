/*
Name : VINAY M R
Date : 22/2/25
Description : Mini Shell Project
*/



//header inclusion
#include "main.h"

//gloabl variables
char *external_commands[200];
int status,cpid = 0;

extern int tstp_pid;
//list to store data for tstp signal
sll *head = NULL;
char input_string[100];


//own signal handler for sigint
void own_sigint_handler(int sigid)
{
    //if child is running
    if(cpid == 0)
    {
        printf("\nminishell$");
        fflush(stdout);
    }else
        //if no parent is running
        printf("\n");
}

//own signal handler for sigtstp
void own_sigtstp_handler(int sigid)
{
    //parent storing information of command
    if(cpid > 0)
    {
        //allocating memory to create new node for new command
        sll *new = malloc(sizeof(sll));
        if(new == NULL)
        {
            printf("Memory allocation for node has failed\n");
            exit(0);
        }
        new -> child_pid = cpid;
        strcpy(new -> command,input_string);
        new -> link = NULL;

        //if list is empty
        if(head == NULL)
            head = new;
        else{
            //if list is not empty
            sll *temp = head;
            while(temp -> link != NULL)
                temp = temp -> link;
            temp -> link = new;
        }

        //terminating the child process by parent
        kill(cpid,SIGKILL);
        printf("\n");
/*      
        sll *temp = head;
        while(temp != NULL)
        {
            printf("pid = %d and command = %s\n",temp -> child_pid,temp -> command);
            temp = temp -> link;
        }
*/
    }
    else if(tstp_pid > 0)
    {
        //for resume commands if child is not running
        kill(tstp_pid,SIGKILL);
        printf("\n");
    }
    else{
        //if child is not running
        printf("\nminishell$");
        fflush(stdout);
    }
}




int main()
{
    char prompt[100];
    //promt for to receive commands
    char *p	= "minishell$";
    strcpy(prompt,p);
    //system call to clear to seem like our prompt is the original terminal
    system("clear");
    //registering the signals
    signal(SIGINT,own_sigint_handler);
    signal(SIGTSTP,own_sigtstp_handler);
    //function call
    scan_input(prompt,input_string);
}


void scan_input(char *prompt,char *input_string)
{
    //extract the external commands to 2D array
    extract_external_commands(external_commands);
    //forever loop to run as if bash
    while(1)
    {
        //prompt printing and waiting for input
        printf("%s",prompt);
        scanf("%[^\n]",input_string);
        __fpurge(stdin);

        //check if the command is given for resuming the given stopped process
        if(check_for_commands_resume(input_string)){}
        //check for PS1 to change the prompt
        else if(strstr(input_string,"PS1="))
        {
            //just modifying the data in the string
            char *addr = strstr(input_string,"=");
            memset(prompt,0,sizeof(prompt));
            strcpy(prompt,addr + 1);
        }//check for the pipe commands to give multiple commands
        else if(strstr(input_string,"'|'")){
            //execute the mutiple process and provide last output
            execute_npipe(input_string);
        }else{
            //get the command given as input function call
            char *command = get_command(input_string);
            //check for command type function call
            int ret = check_command_type(command);
            if(ret == EXTERNAL)
            {
                //if external command create child and execute the command for running the parent same without terminating
                cpid = fork();
                if(cpid > 0)
                {
                    //wait till the child terminates
                    wait(&status);
                    cpid = 0;
                }else if(cpid == 0)
                {
                    //execute the external commands in child process
                    execute_external_commands(input_string);
                }
            }else if(ret == BUILTIN)
            {
                //executing the builtin commands
                execute_internal_commands(input_string);
            }
            //exit(0);
        }
        //setting the input string to zeros to avoid any compications in next inputs
        memset(input_string,0,sizeof(input_string));
    }
}

void extract_external_commands(char *external_commands[200])
{
    //pe the external commands availble fileto execute
    int fd = open("ext_commands.txt",O_RDONLY);
    if(fd == -1)
    {
        //if file is not open print error message
        perror("open");
        exit(0);
    }
    //if opened read all data to 2D array
    int k = 0,ret = 0;
    do{
        char ch = 0;
        int i = 0;
        external_commands[k] = malloc(sizeof(char [40]));
        while((ret = read(fd,&ch,1)) && ch != '\n')
        {
            external_commands[k][i++] = ch;
        }
        if(ret > 0)
        {
            external_commands[k][i] = '\0';
            //printf("ret = %d & %s\n",ret,external_commands[k]);
            k++;
        }else{
            external_commands[k] = NULL;
        }
    }while(ret != 0);
}


char *get_command(char *input_string)
{
    int i = 0;
    //allocate the memory for the string 
    char *new_str = calloc(sizeof(input_string),sizeof(char));
    if(new_str == NULL)
    {
        printf("Error: Allocating for the new_str has been failed!\n");
        return NULL;
    }
    //extract the first command of the string to check command type
    while(input_string[i] != ' ' && input_string[i] != '\0')
    {
        new_str[i] = input_string[i];
        i++;
    }
    new_str[i] = '\0';
    //reducing to required memory level
    new_str = realloc(new_str,(i + 1) * sizeof(char));
    //printf("new string = %s\n",new_str);
    return new_str;
}

int check_command_type(char *command)
{
    //builtin commands available array
    char *builtins[] = {"echo", "printf", "read", "cd", "pwd", "pushd", "popd", "dirs", "let", "eval",
        "set", "unset", "export", "declare", "typeset", "readonly", "getopts", "source",
        "exit", "exec", "shopt", "caller", "true", "type", "hash", "bind", "help", NULL};
    int i = 0;
    //check if the given command is builtin command
    while(builtins[i] != NULL)
    {
        if(!strcmp(builtins[i],command))
            return BUILTIN;
        i++;
    }
    //for(int i = 0; i < size; i++)
    //printf("%s\n",external_commands[i]);
    i = 0;
    //check if the command is external command 
    while(external_commands[i] != NULL)
    {
        if(!strcmp(external_commands[i],command))
            return EXTERNAL;
        i++;
    }
    //if not both type
    return NO_COMMAND;
}
