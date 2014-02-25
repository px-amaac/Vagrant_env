#include <stdio.h>
#include <unistd.h>

int main()
{
	int pid1, pid2, pid3;
	pid1 = fork();
	if(pid1 == 0)
		printf("1st child process with pid %d\n", getpid());
	else
		printf("I am the parent process %d\ncreating my child %d\n", getpid(), pid1);
	pid2 = fork();
	if(pid2 == 0)
		printf("2nd child process with pid %d\n", getpid());
	else
		printf("I am the parent process %d\ncreating my child %d\n", getpid(), pid2);
	pid3 = fork();
	if(pid3 == 0)
		printf("3rd child process with pid %d\n", getpid());
	else
		printf("I am the parent process %d\ncreating my child %d\n", getpid(), pid3);

}
