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

int getCommand(char* arguments[]) {
	printf("$ ");
	char* line = NULL;
	char* token = NULL;
	int read = 0;
	size_t length = 0;
	read = getline(&line, &length, stdin);
	if (read == -1) {
		exit(1);
	}
	if (line[strlen(line) - 1] == '\n') {
		line[strlen(line) - 1] = '\0';
	}
	int index = 0;
	while ((token = strsep(&line, " ")) != NULL) {
		arguments[index] = token;
		index++;
	}
	arguments[index] = NULL;
//	printf("getCommand: ");
//	for (index = 0; arguments[index] != NULL; index++) {
//		printf("%s ", arguments[index]);
//	}
//	printf("\n");
	return 0;
}

int execute(char* arguments[]) {
	int index = 0;
	for (; arguments[index] != NULL; index++) {
		if (strcmp(arguments[index], "<") == 0) {
//			printf("child: input redirection!\n");
			int fdin = open(arguments[index+1], O_RDONLY);
			dup2(fdin, 0);
			close(fdin);
			break;
		} else if (strcmp(arguments[index], ">") == 0) {
//			printf("child: output redirection!\n");
			FILE* garbage = fopen(arguments[index+1], "w+");
			fclose(garbage);
			int fdout = open(arguments[index+1], O_WRONLY | O_CREAT, 0600);
			dup2(fdout, 1);
			close(fdout);
			break;
		} else if (strcmp(arguments[index], ">>") == 0) {
//			printf("child: output append redirection!\n");
			int fdout = open(arguments[index+1], O_WRONLY | O_APPEND);
			dup2(fdout, 1);
			close(fdout);
			break;
		}
	}
	arguments[index] = NULL;
	execvp(arguments[0], arguments);
	return 0;
}

int makeChildWork(char* arguments[]) {
//	printf("makeChildWork: initiating\n");
	int pid;
	int status;
	pid = fork();
	if (pid < 0) { // fork() may fail
		perror("fork faild");
		exit(1);
	}
	if (pid != 0) { // parent
//		printf("parent: pid = %d; ppid = %d\n", getpid(), getppid());
		pid = wait(&status);
//		printf("parent: dead_child.pid = %d, status = %04x\n", pid, status);
	} else { // child
		execute(arguments);
	}
	return 0;
}

int mysh() {
	char* arguments[100];
	getCommand(arguments);
	if (strcmp(arguments[0], "cd") == 0) {
		if (arguments[1] != NULL) {
//			printf("arguments[1] = %s\n", arguments[1]);
			chdir(arguments[1]);
		} else {
//			printf("HOME = %s\n", getenv("HOME"));
			chdir(getenv("HOME"));
		}
	} else if (strcmp(arguments[0], "exit") == 0) {
		exit(1);
	} else {
		makeChildWork(arguments);
	}
	return 0;
}

int main() {
	int iteration = 0;
	for (; iteration < 100; iteration++) {
		mysh();
	}
	return 0;
}
