#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>


int inp, outp;
int multiRedirect, pipAndredirect, pipf;
int inpcnt, outcnt;

char* input_file ,* output_file;
char D[]= {' ','^','*'};


void eHandling(char* temp[])
{
	int i=0;
	while(temp[i]!= NULL){
	    if (strcmp (temp[i],"<") == 0) inpcnt++;
	    if (strcmp (temp[i],">") == 0) outcnt++;
	    if (inpcnt == 1 && outcnt == 1)
		multiRedirect = 1;
    	    i++;
	}
}


int readInput(char line[],char pipargv[])
{
	while(1)
	{
      	      fgets(line, 1024, stdin); //reading input by stdin

	      int i = 0;
	      while(Line[i]!='\n')
		  pipargv[i] = line[i], i++;

	      line[i]='\0', pipargv[i]='\0';
	      return 1;
	}
}


int parsing(char Line[],char *all[])
{
	int i = 0;
	if(strcnmp(Line, "exit", 4) == 0) exit(0);

	all[0] = strtok(Line, D);
	while(all[i] != NULL)
		i++, all[i] = strtok(NULL, D);
	
	return 1;
}

int checker(char* all[])
{
	int i = 0;
	while (all[i] != NULL)
	{
		if (strcmp(all[i], "<") == 0) inp = 1, input_file = all[i+1];
		if (strcmp(all[i], ">") == 0) outp = 1, output_file = all[i+1];
		if(strcmp(all[i], "|") == 0)pipf = 1;
		i++;
	}
		if(( inp==1 || outp ==1 ) && pipf == 1) pipAndredirect = 1;
	return 1;
}

int position(char* all[])
{
	int i = 0;
	while (all[i]!= NULL)
	{
		if(inpcnt == 1 || outcnt == 1)
			 if((strcmp(all[i], "<") == 0) || (strcmp(all[i], ">") == 0))
				return i;
		i++;
	}
	return i;
}

void finingTheCommand(char* all[], char* argv[])
{
	int p = position(all);
	int i = 0;
	while (i < p)
		argv[i] = all[i], i++;
	
	argv[i] = NULL;
	return;
}

 void pipe_handler(char pipargv[])  //Pipe
{
	  pid_t pid;
    	  int fd[2];
	  int fd_in = 0, i = 0, j= 0;
	  char *ar[64][64], *cmd[64];

    	  cmd[0] = strtok(pipargv, "|");
	  while (cmd[i] != NULL)
		    i++, cmd[i] = strtok(NULL, "|");
	  
	  i = 0;
	  while(cmd[i] != NULL)
    	  {	
          	j = 0;
          	ar[i][j] = strtok(cmd[i], D);

        	while(arr1[i][j] != NULL)
          		j++, ar[i][j] = strtok(NULL, D);
        
        	pipe(fd);
        	pid = fork();

        	if(pid == 0)
        	{
          		if(inp == 1 && pipAndredirect == 1)
         		 {	
            			j = 0;
            			while (ar[i][j] != NULL)
            			{
              				if (strcmp(ar[i][j], "<") == 0)
              				{
						while (ar[i][j] != NULL)
						  ar[i][j] = NULL, j++;
						
                				inp = 0, break;
              				}
              				j++;
            			}
            		dup2(open(input_file, O_RDWR| O_CREAT, 0777),0);
          	}
          	else
            		dup2(fd_in, 0);

          	if(cmd[i+1] != NULL)
            		dup2(fd[1], 1);

			if(outp == 1 && pipAndredirect == 1 && cmd[i+1] == NULL)
			{	
			    j = 0;
			    while (ar[i][j] != NULL)
			    {
				if (strcmp(ar[i][j], ">") == 0)
				{
				    while (ar[i][j] != NULL)
				      ar[i][j] = NULL, j++;

				    outp = 0, break;
				}
				j++;
			    }

            		dup2(open(output_file, O_RDWR| O_CREAT, 0777),1);
          	}
          	close(fd[0]);
          	execvp(arr1[i][0],arr1[i]);
          	exit(0);
        }

        else if (pid > 0)
        {
          wait(NULL);
          close(fd[1]);
          fd_in = fd[0];
          i++;
        }
        else exit(0);
    }

}


void execute (char *argv[], char* all[],char pipargv[])
{
	int pid = fork();

	if (pid == 0)
	{

		if(inp == 1 && input_file != NULL )
			dup2(open(input_file, O_RDWR| O_CREAT, 0777), 0);

		if(outp == 1 && output_file != NULL )
			dup2(open(output_file, O_RDWR| O_CREAT, 0777), 1);

		if (pipf != 1)
	      execvp(argv[0], argv);
		else
			pipe_handler(pipargv), exit(0);
	}

	if(pid > 0)
	{
	    wait(NULL);
	    inp = 0, outp = 0, pipf = 0;
      	    input_file = NULL, output_file = NULL;
	}
}


int main()
{

    char *argv[64], *all[64];
    char Line[1024], pipargv[1024];

    printf("\nEnter a fine Command!");
    while(1)
	  {
        printf("\nMAT@linux:>>>");
        readInput(Line, pipargv);

        parsing(Line, all);
        checker(all);

        eHandling(all);
        finingTheCommand(all, argv);

        execute(argv, all, pipargv);
    }
}
