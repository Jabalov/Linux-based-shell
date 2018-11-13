#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>




int inp, outp;
int multi_redirect, pipAndredirect, pipf;
int inpcnt, outcnt;

char* input_file ,* output_file;
char D[]= {' ','^','*'};


void countRP(char* all[])
{
	int i=0;
	while(all[i]!= NULL){
    if (strcmp (all[i],"<") == 0) inpcnt++;
    if (strcmp (all[i],">") == 0) outcnt++;
    if (inpcnt == 1 && outcnt == 1)
        multi_redirect = 1;
    i++;
	}
  
}


int readInput(char Line[],char Line1[])
{
	while(1)
	{
      fgets(Line, 1024, stdin);

      int i = 0;
      while(Line[i]!='\n')
          Line1[i]=Line[i], i++;
            
      Line[i]='\0', Line1[i]='\0';
      return 1;
	}
}

char *directory;
int parsing(char line[],char *all[])
{

	int i=0;
	if(strcmp(line, "exit")==0) 
    exit(0);
  if(strcmp(line, "cd") == 0)
  {
			directory = line[1];
			chdir(directory);
			if(chdir(directory) == -1)
				fprintf(stderr, "shell: Error changing directory\n");	
	}

	all[i] = strtok(line, D);
	while(all[i] != NULL)
		i++, all[i] = strtok(NULL, D);
	
	return 1;
}

int checkRP(char* temp[])
{
	int i=0;

	while (temp[i] != NULL)
	{
		if (strcmp(temp[i], "<") == 0)
			inp = 1, input_file = temp[i+1];

		if (strcmp(temp[i], ">") == 0)
			outp = 1, output_file = temp[i+1];
		
		if(strcmp(temp[i], "|") == 0)
			pipf = 1;
		i++;
	}
		if((inp == 1 || outp == 1) && pipf == 1)
			pipAndredirect = 1;
				
	return 1;
}

int getPos(char* temp[])
{
	int i = 0;
	while (temp[i]!= NULL)
	{
		if(inpcnt == 1 || outcnt == 1)
		{
			 if((strcmp(temp[i], "<") == 0) || (strcmp(temp[i], ">") == 0))
				return i;
		}

		i++;
	}
	return i;
}

void fineCMD(char* all[], char* argv[])
{
	int p = getPos(all);
	int i = 0;
	while (i < p)
		argv[i] = all[i], i++;
	
	argv[i] = NULL;
	return;
}

 void pipeHandle(char pipargv[])  //Pipe
{
	  int status2, pid;
    int fd[2];
	  int fd_in = 0, i = 0, j= 0;
	  char *arr1[64][64], *cmd[64];


    cmd[0] = strtok(pipargv, "|");
	  while (cmd[i] != NULL)
		    i++, cmd[i] = strtok(NULL, "|");
	  

	  i=0;

  while(cmd[i] != NULL)
    {	
        j=0;
        arr1[i][j] = strtok(cmd[i], D);

        while(arr1[i][j]!=NULL)
        {
          j++;
          arr1[i][j] = strtok(NULL, D);
        }

        pipe(fd);
        pid = fork();

        if(pid == 0)
        {
          if(inp == 1 && pipAndredirect == 1)
          {	
            j=0;
            while (arr1[i][j] != NULL)
            {
              if (strcmp(arr1[i][j], "<") == 0)
              {
                while (arr1[i][j] != NULL)
                {
                  arr1[i][j]=NULL;
                  j++;
                }
                inp = 0;
                break;

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
            while (arr1[i][j] != NULL)
            {
                if (strcmp(arr1[i][j], ">") == 0)
                {
                    while (arr1[i][j] != NULL)
                    {
                      arr1[i][j] = NULL;
                      j++;
                    }
                    outp = 0;
                    break;

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
			pipeHandle(pipargv), exit(0);
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
        checkRP(all);

        countRP(all);
        fineCMD(all, argv);

        execute(argv, all, pipargv);

    }
}
