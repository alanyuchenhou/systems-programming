#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#define MAX 10000
typedef struct {
	char *name;
	char *value;
} ENTRY;

ENTRY entry[MAX];
int copy (FILE* source, FILE* destination) {
	if (source == NULL || destination == NULL) {
		return -1;
	}
	char * buffer = 0;
	size_t length = 0;
	while (getline(&buffer, &length, source) != -1) {
		fputs(buffer, destination);
	}
	return 0;
}
int main4() {
	char cwd[128];
	entry[0].name = "command";
	entry[0].value = "ls";
	entry[1].name = "file1";
	entry[1].value = ".";
	entry[2].name = "file2";
	entry[2].value = "f1";
	getcwd(cwd, 128); // get CWD pathname

//	printf("Content-type: text/html\n\n");
//	printf("<p>pid=%d uid=%d cwd=%s\n", getpid(), getuid(), cwd);
//	printf("<H1>Inputs</H1>");
//	printf("You submitted the following name/value pairs:<p>");
//
//	for (i = 0; i <= m; i++)
//		printf("%s = %s<p>", entry[i].name, entry[i].value);
//	printf("<H1>Outputs</H1>");
	/*****************************************************************
	 Write YOUR C code here to processs the command
	 mkdir dirname
	 rmdir dirname
	 rm    filename
	 ls    [dirname] <== ls CWD if no dirname
	 cat   filename
	 cp    file1 file2
	 *****************************************************************/
	int status = -2;
	if (strcmp(entry[0].value, "mkdir") == 0) {
		status = mkdir(entry[1].value, 0777);
	} else if (strcmp(entry[0].value, "rmdir") == 0) {
		status = rmdir(entry[1].value);
	} else if (strcmp(entry[0].value, "rm") == 0) {
		status = remove(entry[1].value);
	} else if (strcmp(entry[0].value, "ls") == 0) {
		DIR* dir = NULL;
		if (strcmp(entry[1].value, "") == 0) {
			dir = opendir(".");
		} else {
			dir = opendir(entry[1].value);
		}
		struct dirent* dirEntry;
		if (dir) {
			while ((dirEntry = readdir(dir)) != NULL) {
//				printf("%s<p>", dirEntry->d_name);
				printf("%s\n", dirEntry->d_name);
			}
			closedir(dir);
			status = 0;
		}
	} else if (strcmp(entry[0].value, "cat") == 0) {
		FILE* source = fopen(entry[1].value, "r");
		status = copy(source, stdout);
		fclose(source);
	} else if (strcmp(entry[0].value, "cp") == 0) {
		FILE* source = fopen(entry[1].value, "r");
		FILE* destination = fopen(entry[2].value, "w+");
		status = copy(source, destination);
		fclose(source);
		fclose(destination);
	}
	printf("status = %d\n", status);
	return 0;
}
