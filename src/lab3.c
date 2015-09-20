/*
 *      Author: Yuchen Hou
 *      ID: 11388981
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <assert.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

int main() {
	printf("$ ");
	char* line = NULL;
	char* token;
	int read;
	size_t length;
	read = getline(&line, &length, stdin);
	if (read == -1) {
		exit(1);
	}
	if (line[strlen(line) - 1] == '\n') {
		line[strlen(line) - 1] = '\0';
	}
	char* strings[9];
	int index = 0;
	while ((token = strsep(&line, " ")) != NULL) {
		strings[index] = token;
		index++;
	}
	strings[index] = NULL;
	for (index = 0; strings[index] != NULL; index++) {
		printf("%s\n", strings[index]);
	}
	int pid;
	int status;
	pid = fork(); // fork syscall; parent returns child pid; child returns 0

	if (pid < 0) { // fork() may fail. e.g. no more PROC in Kernel
		perror("fork faild");
		exit(1);
	}

	if (pid) { // parent
		printf("parent: child.pid = %d; pid = %d; ppid = %d\n", pid, getpid(),
				getppid());
		pid = wait(&status);
		printf("parent: dead_child.pid = %d, status = %04x\n", pid, status);

	} else { // child
		printf("child: pid = %d; ppid = %d\n", getpid(), getppid());
		execl("ls", "ls", NULL);
		printf("child: dying\n");
		exit(100); //OR {int a,b; a=b/0;} ==> see how does it die

		close(0); // system call to close file descriptor 0
		open("filename", O_RDONLY); // open filename for READ, which
		close(1);
		open("filename", O_WRONLY | O_CREAT, 0644);
	}
	return 0;
}
