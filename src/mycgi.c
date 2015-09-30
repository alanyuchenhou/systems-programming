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
int copy(FILE* source, FILE* destination) {
	if (source == NULL || destination == NULL) {
		return -1;
	}
	char * buffer = 0;
	size_t length = 0;
	while (getline(&buffer, &length, source) != -1) {
		fputs(buffer, destination);
	}
	free(buffer);
	return 0;
}
int main(int argc, char *argv[]) {
//	int i, m, r;
	char cwd[128];

//	m = getinputs(); // get user inputs name=value into entry[ ]
	if (argc > 0) {
		entry[0].value = argv[1];
	}
	if (argc > 1) {
		entry[1].value = argv[2];
	}
	if (argc > 2) {
		entry[2].value = argv[3];
	}
	getcwd(cwd, 128); // get CWD pathname

//	printf("Content-type: text/html\n\n");
//	printf("<p>pid=%d uid=%d cwd=%s\n", getpid(), getuid(), cwd);
//	printf("<H1>Inputs</H1>");
//	printf("You submitted the following name/value pairs:<p>");

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
	// create a FORM webpage for user to submit again
//	printf("</title>");
//	printf("</head>");
//  printf("<body bgcolor=\"#FF0000\" link=\"#330033\" leftmargin=8 topmargin=8");
//	printf("<p>------------------ DO IT AGAIN ----------------\n");
//	printf(
//			"<FORM METHOD=\"POST\" ACTION=\"http://cs360.eecs.wsu.edu/~hou/cgi-bin/mycgi\">");
//	printf("Enter command : <INPUT NAME=\"command\"> <P>");
//	printf("Enter filename1: <INPUT NAME=\"filename1\"> <P>");
//	printf("Enter filename2: <INPUT NAME=\"filename2\"> <P>");
//	printf(
//			"Submit command: <INPUT TYPE=\"submit\" VALUE=\"Click to Submit\"><P>");
//	printf("</form>");
//	printf("------------------------------------------------<p>");
//
//	printf("</body>");
//	printf("</html>");
	return 0;
}
