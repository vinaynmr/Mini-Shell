#include "main.h"

//static variables using only for pipe process
static int pipe_count,*pos_arr,size;
//extern linkng
extern int status;

//execute the pipe
void execute_npipe(char *input_string)
{
    //function to get an 2D of the commands
    char **argv = prepare_for_piping(input_string);
    int fd[2];

    //take bakup of input and output std files atlast we have to fix their position to run forever
    int stdout_pos = dup(1);
    int stdin_pos = dup(0);

    //loop for executing pipes
    for(int i = 0; i < pipe_count; i++)
    {
        //create a pipe to give output to next process
        if(i < (pipe_count - 1))
            pipe(fd);

        //change back position of the stdout file to use again after pipe execution
        if(i == (pipe_count - 1))
        {    
            dup2(stdout_pos,1);
        }

        //create a child to execute the pipe commands
        int pid = fork();
        //for parent
        if(pid > 0)
        {
            //rearrange the fd positions as per the usage of the pipe execution
            if(i < (pipe_count - 1))
            {
                dup2(fd[0],0);
                close(fd[1]);
                close(fd[0]);
            }
            //wait for the child to execute
            wait(&status);
            //print the exit status of the child if abnormally exited
            if(!WIFEXITED(status))            
            {
                printf("Exit status of child %d\n",WEXITSTATUS(status));
                printf("Error: cannot find file after '|'\n");
            }
        }//execute the command using child
        else if(pid == 0)
        {
            //rearrange the write end of the pipe to stdout to give as input to next file
            if(i < (pipe_count - 1))
            {
                dup2(fd[1],1);
                close(fd[0]);
            }

            //execute the commands
            execvp(argv[pos_arr[i]],argv + pos_arr[i]);
            exit(0);
        }
    }

    //fix the stdin position back for further use
    dup2(stdin_pos,0);
    //close pipe ends after use
    close(fd[0]);            
    close(fd[1]);
}


char **prepare_for_piping(char *input_string)
{
    //allocate memory for the array
    char **argv = malloc(100 * sizeof(char *));
    char delim = ' ';
    //use strtok to get the commands
    char *token = strtok(input_string,&delim);
    int k = 0;
    while(token)
    {
        argv[k] = malloc(40 * sizeof(char));
        strcpy(argv[k],token);
        //use strtok to get the commands
        token = strtok(NULL,&delim);
        k++;
    }
    //get the actual required memory and free unused
    argv = realloc(argv,(k + 1) * sizeof(char *));
    size = k + 1;
    int i = 0,z = 1;

    //store the commands starting positions to the array 
    pos_arr = malloc(k * sizeof(int));
    pos_arr[z - 1] = 0;
    
    while(argv[i] != NULL)
    {
        //store each command starting position
        if(!strcmp(argv[i],"'|'"))
        {
            z++;
            pos_arr = realloc(pos_arr,z * sizeof(int));
            pos_arr[z - 1] = i + 1;
            argv[i] = NULL;
        }
        i++;
    }
    /*
    for(int i = 0; i < z; i++)
        printf("%d\n",pos_arr[i]);
    */
    pipe_count = z;
    //printf("pipe_count = %d\n",pipe_count);
    return argv;
}


