#include<stdio.h>
#include<sys/types.h>
//#include<functions.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#define PERMS 0644
char* get_line(FILE *f)
{
	char* line = NULL;
	char c;
	int len = 0;
	c = fgetc(f);
	while(c != EOF && c != '\n')
	{
		line = (char*) realloc(line,sizeof(char) * (len + 2));
		line[len++] = c;
		line[len] = '\0';
		c = fgetc(f);
		
	}
	return line;
}
int flag;
int if_pipe;
char file1[200] = "stdin";
char file2[200] = "stdout";
int cnt;
char** parse(char* line)
{
	if_pipe = -1;
	char** args = NULL;
	char* word = NULL;	
	cnt = 0;
	int len = 0; 
	int i;
	strcpy(file2, "stdout");
	strcpy(file1,"stdin");
	for(i = 0 ;; i++)
	{	
		if(line[i] == ' ' || line[i] == '\0')
		{
			if(word == NULL)
				continue;			
			args = (char**)realloc(args,sizeof(char*) * (cnt + 1));
			if(strcmp(word,"|") == 0)
				if_pipe = cnt;
			if(strcmp(word,"&") == 0)
				 flag = 1;
			else			
				args[cnt++] = word;
			if(cnt > 1 && strcmp(args[cnt - 2],"<") == 0)
			{
				int j;
				for(j = 0 ; word[j] != '\0'; j++)
					file1[j] = (char)word[j];
				file1[j] = '\0';
				args[cnt - 1] = NULL;
				args[cnt - 2] = NULL;
				cnt -= 2;
				args = (char**)realloc(args,sizeof(char*) * (cnt));
			}
			if(cnt > 1 && strcmp(args[cnt - 2],">") == 0)
			{
				int j;
				for(j = 0 ; word[j] != '\0'; j++)
					file2[j] = word[j];
				file2[j] = '\0';

				args[cnt - 1] = NULL;
				args[cnt - 2] = NULL;
				cnt -= 2;
				args = (char**)realloc(args,sizeof(char*) * (cnt));
			}
			word = NULL;
			len = 0; 
		}
		else
		{
			word = (char*)realloc(word,sizeof(char) * (len + 2));
			word[len++] = line[i];
			word[len] = '\0';
		}
		if(line[i] == '\0')
			break;
	}
	return args;
}

void perform_pipe(char **args)
{
	char **args1 = NULL;
	char **args2 = NULL;
	int cnt1 = 0, cnt2 = 0;
	int i;	
	int stat;
	int fd[2];
	for(i = 0 ; i < if_pipe; i++)
	{		
			args1 = (char**)realloc(args1,sizeof(char*) * (cnt1 + 1));
			args1[cnt1++] = args[i];
	}		
	for(i = if_pipe + 1 ; i < cnt; i++)
	{
			args2 = (char**)realloc(args2,sizeof(char*) * (cnt2 + 1));
			args2[cnt2++] = args[i];
	}
	args1[cnt1] = NULL;
	args2[cnt2] = NULL;
	if(pipe(fd) < 0)
	{
		printf("Cannot open the pipe\n");
		exit(1);
	}
	pid_t pd1,pd2;
	if((pd1 = fork()) == 0)
	{
		close(fd[0]);
		dup2(fd[1],1);
		//close(fd[0]);
		close(fd[1]);
		printf("Hello from Parent 1\n");
		execvp(args1[0],args1);
		exit(20);
	}
	if((pd2 = fork()) == 0)
	{
		close(fd[1]);
		dup2(fd[0],0);
		close(fd[0]);
		//close(fd[1]);
		printf("Hello from Parent 2\n");
		execvp(args2[0],args2);
		exit(20);
	}	
	//waitpid(pd2,&stat,0);
	//waitpid(pd1,&stat,0);
}


int main()
{
	char* line;
	char** args;
	int stat;
	int fd0, fd1;
	while(1)
	{
		flag = 0;
		//fflush(stdout);
		//fflush(stdin);
		printf("Ana Gahez ya Basha >> ");

		line = get_line(stdin);
		if(line == NULL || (args = parse(line)) == NULL)
			continue;
		int x = strcmp(args[0],"exit");
		if(strcmp(file2,"stdout") != 0)
		{
			if((fd1 = open(file2,O_CREAT|O_TRUNC|O_WRONLY,PERMS)) == -1)
			{
				printf("Error, the file cannot be opened\n");
				continue;
			}
		}
		if(strcmp(file1,"stdin") != 0)
		{
			if((fd0 = open(file1,O_RDONLY)) == -1)
			{
				printf("Error, the file cannot be opened\n");
				continue;
			}
		}
		if(x == 0)
			exit(0);
		if(if_pipe != -1)
		{
			args[cnt] = NULL;
			if(fork() == 0)
			{
				execvp("./mypipe",args);
			}
			wait(&stat);
			//perform_pipe(args);
			continue;
		}
		if(fork() == 0) // To be fixed !!
		{
			if(strcmp(file2,"stdout") != 0)
			{
				close(1);
				dup(fd1);
				close(fd1);
			}
			if(strcmp(file1,"stdin") != 0)
			{
				close(0);
				dup(fd0);
				close(fd0);
			}
			args[cnt] = NULL;
			execvp(args[0],args);
			printf("Unknown Command !!\n");
			exit(1);
		}
		if(flag == 0)
		{
			wait(&stat);
		}
		fflush(stdin);
		fflush(stdout);
	}
	return 0;
}
