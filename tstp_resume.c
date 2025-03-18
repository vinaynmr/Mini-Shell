#include "main.h"

//linking of the global variables
extern sll *head;
extern int status;
//global variable
int tstp_pid = 0;


//check for the commands which wants to resume the stopped processes
int check_for_commands_resume(char *input_string)
{
    //check for the fg command execute the first stopped process
    if(!strcmp(input_string,"fg"))
    {
        //if list is empty no process stopped available
        if(head == NULL)
            return SUCCESS;
        //if available execute it
        sll *temp = head;
        head = head -> link;
        //create a child to execute command
        tstp_pid = fork();
        if(tstp_pid > 0)
        {
            //wait for child to execute command
            wait(&status);
            tstp_pid = 0;
        }else if(tstp_pid == 0)
        {
            //extract the command into 2D array
            char *str[3];
            char delim = ' ';

            //extract each command as tokens by strtok
            char *token = strtok(temp -> command,&delim);
            int k = 0;
            //extract each command as tokens by strtok
            while(token)
            {
                str[k] = malloc(40 * sizeof(char));
                strcpy(str[k],token);
                token = strtok(NULL,&delim);
                //printf("%s\n",str[k]);
                k++;
            }
            str[3] = NULL;
            //printf("3rd = %s\n",str[3]);

            //execute the command
            execvp(str[0],str);
            printf("Execution has failed!\n");
            //if execution failed terminate child because it has no use now
            exit(0);
        }
        return SUCCESS;
    }//check for the fg command execute the first stopped process
    else if(!strcmp(input_string,"bg"))
    {
        //if list is empty no process stopped available
        if(head == NULL)
            return SUCCESS;
        //if available execute it
        sll *temp = head;
        head = head -> link;
        
        //create a child to execute command
        tstp_pid = fork();
        if(tstp_pid > 0)
        {
            //don't need to wait for child because it is bg command both execution work parallely
            usleep(500);
            tstp_pid = 0;
        }else if(tstp_pid == 0)
        {
            //extract the command into 2D array
            char *str[3];
            char delim = ' ';

            //extract token of each commands to execute
            char *token = strtok(temp -> command,&delim);
            int k = 0;
            while(token)
            {
                str[k] = malloc(40 * sizeof(char));
                strcpy(str[k],token);
                //extract token of each commands to execute
                token = strtok(NULL,&delim);
                //printf("%s\n",str[k]);
                k++;
            }
            str[3] = NULL;
            //printf("3rd = %s\n",str[3]);

            //execute commands
            execvp(str[0],str);
            printf("Execution has failed!\n");
            //if execution failed terminate child because it has no use now
            exit(0);
        }
        return SUCCESS;
    }else if(!strcmp(input_string,"jobs"))
    {
        //for jobs we just have to print the stopped processes
        sll *temp = head;
        int l = 1;
        while(temp != NULL)
        {
            printf("[%d] stopped             %s\n",l,temp -> command);
            temp = temp -> link;
        }
        return SUCCESS;
    }
    return FAILURE;
}
