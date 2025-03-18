//header inclusion
#include "main.h"

//executing the external command
void execute_external_commands(char *input_string)
{
    //get the string into multiple indices of 2D array to execute easily
	char *argv[100];
	int i = 0, k = 0, j = 0;
	argv[j] = calloc(40,sizeof(char));
    //is memory not allocated
	if(argv[j] == NULL)
	{
		printf("Error: Memory allocation has been failed!\n");
	}
    //extract each commands into a 2D array
	while(input_string[i] != '\0')
	{
		if(input_string[i] == ' ')
		{
			argv[j][k] = '\0';
			j++;
			k = 0;
			argv[j] = calloc(40,sizeof(char));
			if(argv[j] == NULL)
			{
				printf("Error: Memory allocation has been failed!\n");
			}
		}else{
			argv[j][k++] = input_string[i];
		}
		i++;
	}
	argv[j][k] = '\0';
	argv[++j] = NULL;
    /*
	for(int l = 0; l <= j; l++)
	{
		printf("2D => %s\n",argv[l]);
	}
    */

    //executing the commands
	execvp(argv[0],argv);
    exit(0);
}



//executing the internal commands
void execute_internal_commands(char *input_string)
{
    //check for command confirmation
	if(!strcmp(input_string,"exit"))
	{
		//exit from the mini shell
		exit(0);
	}else if(!strcmp(input_string,"cd"))
	{
		char path[200];
		scanf("%[^\n]",path);
		//change the working directory
		chdir(path);
	}else if(!strcmp(input_string,"pwd"))
	{
		char path[200];
		getcwd(path,100);
		printf("%s\n",path);
	}else if(strstr(input_string,"echo"))
	{
		//echo $$
		if(!strcmp(input_string,"echo $$"))
		{
			printf("%d\n",getpid());
		}
		//echo $?
		else if(!strcmp(input_string,"echo $?"))
		{
			int status;
			waitpid(getpid(),&status,WNOHANG);
			printf("%d\n",status);
		}
		//echo $SHELL
		else if(!strcmp(input_string,"echo $SHELL"))
		{
			char *env = getenv("SHELL");
			int i = 0;
				printf("%s\n",env);
			/*
			while(env[i] != NULL)
			{
				printf("%s\n",env);
				i++;
			}*/
		}else{
			printf("\nInvalid Command: Please give proper command\n");
		}
	}
}

