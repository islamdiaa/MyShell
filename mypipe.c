#include<stdio.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>

int main(int argc,char **args)
{
	char **args1 = NULL;
	char **args2 = NULL;
	int cnt1 = 0, cnt2 = 0;
	int i;	
	int if_pipe,cnt;
	int stat;
	int fd[2];
	for(i = 0 ; args[i] != NULL; i++)
		if(strcmp(args[i],"|") == 0)
			if_pipe = i;
	cnt = i;
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
	if(pipe(fd) < 0)
	{
		printf("Cannot open the pipe\n");
		exit(1);
	}
	pid_t pd1,pd2;
	if((pd1 = fork()) == 0)
	{
		close(1);
		dup(fd[1]);
		close(fd[0]);
		close(fd[1]);
		execvp(args1[0],args1);
		exit(20);
	}
	wait(&stat);
		close(0);
		dup(fd[0]);
		close(fd[0]);
		close(fd[1]);
		execvp(args2[0],args2);
		exit(20);
		
	
}
