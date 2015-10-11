//**************************** ECHO CLIENT CODE **************************
// The echo client client.c

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

#define MAX 256

// Define variables
struct hostent *hp;
struct sockaddr_in server_addr;

int sock, r;
int SERVER_IP, SERVER_PORT;
char * cmd[] = { "get", "put", "ls", "cd", "pwd", "mkdir", "rmdir", "rm",
		"lcat", "lls", "lcd", "lpwd", "lmkdir", "lrmdir", "lrm", 0 };
char home[256]; // $HOME directory

int client_init(char *argv[]);
int findCommand(char * command);
void menu();
void get(char *name);
void put(char *name);
void ls(char *name);
void cd(char *name);
void pwd();
void smkdir(char *name);
void srmdir(char *name);
void rm(char *name);

void lcat(char *name);
void lls(char *name);
int ls_file(char *fname);
int ls_dir(char *dname);
void lcd(char *name);
void lpwd();
void lmkdir(char *name);
void lrmdir(char *name);
void lrm(char *name);

main(int argc, char *argv[], char *env[]) {
	int n;
	char line[MAX], ans[MAX];

	char dirName[128], fileName[128], dirOrFileName[128];
	char command[64];
	int args_assigned;
	char pathStr[256], path[256];
	char homeStr[256];
	char testStr[5];

	if (argc < 3) {
		printf("Usage : client ServerName SeverPort\n");
		exit(1);
	}

// get HOME variables
	int i = 0;
	for (i = 0; env[i] != NULL; i++) {
		strncpy(testStr, env[i], 4);
		testStr[4] = '\0';

		if (strcmp(testStr, "HOME") == 0) {
			strcpy(homeStr, env[i]);
			strcpy(home, &homeStr[5]);
		}
	}

	client_init(argv);

	while (1) {

		menu();
		printf("input a command : ");
		bzero(line, MAX); // zero out line[ ]
		fgets(line, MAX, stdin); // get a line (end with \n) from stdin
		line[strlen(line) - 1] = 0; // kill \n at end

		if (line[0] == 0) // exit if NULL line
			exit(0);

		printf("\n");

		args_assigned = sscanf(line, "%s %s", command, dirOrFileName);

		if (strcmp(command, "get") == 0 && args_assigned == 2) {
			strcpy(fileName, dirOrFileName);
			printf("command: %s  fileName: %s\n", command, fileName);
		} else if (strcmp(command, "put") == 0 && args_assigned == 2) {
			strcpy(fileName, dirOrFileName);
			printf("command: %s  fileName: %s\n", command, fileName);
		} else if (strcmp(command, "ls") == 0 && args_assigned == 2) {
			strcpy(dirName, dirOrFileName); // need to figure out file or dir
			printf("command: %s  dirName: %s\n", command, dirName);
		} else if (strcmp(command, "cd") == 0 && args_assigned == 2) {
			strcpy(dirName, dirOrFileName);
			printf("command: %s  dirName: %s\n", command, dirName);
		} else if (strcmp(command, "pwd") == 0 && args_assigned == 1) {
			printf("command: %s\n", command);
		} else if (strcmp(command, "mkdir") == 0 && args_assigned == 2) {
			strcpy(dirName, dirOrFileName);
			printf("command: %s  dirName: %s\n", command, dirName);
		} else if (strcmp(command, "rmdir") == 0 && args_assigned == 2) {
			strcpy(dirName, dirOrFileName);
			printf("command: %s  dirName: %s\n", command, dirName);
		} else if (strcmp(command, "rm") == 0 && args_assigned == 2) {
			strcpy(fileName, dirOrFileName);
			printf("command: %s  fileName: %s\n", command, fileName);
		} else if (strcmp(command, "lcat") == 0 && args_assigned == 2) {
			strcpy(fileName, dirOrFileName);
			printf("command: %s  fileName: %s\n", command, fileName);
		} else if (strcmp(command, "lls") == 0) {
			if (args_assigned == 1)
				strcpy(dirOrFileName, "");
		} else if (strcmp(command, "lcd") == 0) {
			if (args_assigned == 1)
				strcpy(dirName, ""); // no dirName input
			else
				strcpy(dirName, dirOrFileName);
		} else if (strcmp(command, "lpwd") == 0 && args_assigned == 1) {
			//printf("command: %s\n", command);
		} else if (strcmp(command, "lmkdir") == 0 && args_assigned == 2) {
			strcpy(dirName, dirOrFileName);
		} else if (strcmp(command, "lrmdir") == 0 && args_assigned == 2) {
			strcpy(dirName, dirOrFileName);
		} else if (strcmp(command, "lrm") == 0 && args_assigned == 2) {
			strcpy(fileName, dirOrFileName);
		} else if (strcmp(command, "quit") == 0) {
			exit(1);
		} else {
//			printf("command %s is invalid or wrong.\n", command);
		}

		// Send ENTIRE line to server
		n = write(sock, line, MAX);
		printf("client: wrote n=%d bytes; line=(%s)\n", n, line);
		//"get", "put", "ls", "cd", "pwd", "mkdir", "rmdir", "rm",
		//"lcat", "lls", "lcd", "lpwd", "lmkdir", "lrmdir", "lrm", "quit"
		int ID = findCommand(command);
		switch (ID) {
		case 0:
			get(fileName);
			break;
		case 1:
			put(fileName);
			break;
		case 2:
			ls(dirOrFileName);
			break;
		case 3:
			cd(dirName);
			break;
		case 4:
			pwd();
			break;
		case 5:
			smkdir(dirName);
			break;
		case 6:
			srmdir(dirName);
			break;
		case 7:
			rm(fileName);
			break;
		case 8:
			lcat(fileName);
			break;
		case 9:
			lls(dirOrFileName);
			break;
		case 10:
			lcd(dirName);
			break;
		case 11:
			lpwd();
			break;
		case 12:
			lmkdir(dirName);
			break;
		case 13:
			lrmdir(dirName);
			break;
		case 14:
			lrm(fileName);
			break;
		}

		// Read a line from sock and show it
		n = read(sock, ans, MAX);
		printf("client: read  n=%d bytes; echo=(%s)\n", n, ans);
	}
}

// clinet initialization code
int client_init(char *argv[]) {
	printf("======= clinet init ==========\n");

	printf("1 : get server info\n");
	hp = gethostbyname(argv[1]);
	if (hp == 0) {
		printf("unknown host %s\n", argv[1]);
		exit(1);
	}

	SERVER_IP = *(long *) hp->h_addr;
	SERVER_PORT = atoi(argv[2]);

	printf("2 : create a TCP socket\n");
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		printf("socket call failed\n");
		exit(2);
	}

	printf("3 : fill server_addr with server's IP and PORT#\n");
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = SERVER_IP;
	server_addr.sin_port = htons(SERVER_PORT);

	// Connect to server
	printf("4 : connecting to server ....\n");
	r = connect(sock, (struct sockaddr *) &server_addr, sizeof(server_addr));
	if (r < 0) {
		printf("connect failed\n");
		exit(1);
	}

	printf("5 : connected OK to \007\n");
	printf("---------------------------------------------------------\n");
	printf("hostname=%s  IP=%s  PORT=%d\n", hp->h_name, inet_ntoa(SERVER_IP),
			SERVER_PORT);
	printf("---------------------------------------------------------\n");

	printf("========= init done ==========\n");
}

// find commend function
int findCommand(char * command) {
	int i = 0;
	while (cmd[i]) {
		//printf("%s ",cmd[i]);
		if (strcmp(command, cmd[i]) == 0)
			return i;
		i++;
	}
	return -1;
}

void menu() {
	printf("\n");
	printf("********************** menu *********************\n");
	printf("* get  put  ls   cd   pwd   mkdir   rmdir   rm  *\n");
	printf("* lcat     lls  lcd  lpwd  lmkdir  lrmdir  lrm  *\n");
	printf("*************************************************\n\n");
}

void get(char *name) {
	FILE* destination = fopen("test_file", "w+");
	char ans[MAX];
	int size;
	if (destination) {
		while (strcmp(ans, "@") != 0) {
			read(sock, ans, MAX);
			fwrite(ans, 1, MAX, destination);
		}
	}
	fclose(destination);
}
void put(char *name) {
}
void ls(char *name) {
	char ans[MAX];
	while (strcmp(ans, "@") != 0) {
		read(sock, ans, MAX);
		printf("%s\n", ans);
	}
}
void cd(char *name) {
}
void pwd() {
}
void smkdir(char *name) {
}
void srmdir(char *name) {
}
void rm(char *name) {
}

void lcat(char *name) {
	FILE *fp;
	int c;
	fp = fopen(name, "r"); // open file
	if (fp == 0) {
		printf("lcat %s was failed.\n", name);
	} else {
		// read whole file
		while ((c = fgetc(fp)) != EOF) {
			putchar(c);
		}
		fclose(fp); // close file
		printf("\nlact %s ok \n", name);
	}
}

void lls(char *name) {
	struct stat mystat, *sp;
	int r;
	char *s;
	char fullName[256], cwd[256];

	s = name;
	if (strcmp(name, "") == 0) //there is no filName input
		s = "./";

	sp = &mystat;
	if (r = lstat(s, sp) < 0) {
		printf("no such file %s\n", s); //exit(1);
	} else {
		strcpy(fullName, s);
		if (s[0] != '/') { // name is relative : get CWD path
			getcwd(cwd, 256);
			strcpy(fullName, cwd);
			strcat(fullName, "/");
			strcat(fullName, s);
		}

		if (S_ISDIR(sp->st_mode))
			ls_dir(fullName);
		else
			ls_file(fullName);

		printf("lls %s ok\n", fullName);
	}
}

int ls_file(char *fname) {
	struct stat fstat, *sp;
	int r, i;
	char ftime[64];
	char *t1 = "xwrxwrxwr-------";
	char *t2 = "----------------";
	char nameBuf[1024];
	size_t len;

	sp = &fstat;

	if ((r = lstat(fname, &fstat)) < 0) {
		printf("can't stat %s\n", fname);
		exit(1);
	}

	if ((sp->st_mode & 0xF000) == 0x8000)
		printf("%c", '-');
	if ((sp->st_mode & 0xF000) == 0x4000)
		printf("%c", 'd');
	if ((sp->st_mode & 0xF000) == 0xA000)
		printf("%c", 'l');

	for (i = 8; i >= 0; i--) {
		if (sp->st_mode & (1 << i))
			printf("%c", t1[i]);
		else
			printf("%c", t2[i]);
	}

	printf("%4d ", sp->st_nlink);
	printf("%4d ", sp->st_gid);
	printf("%4d ", sp->st_uid);
	printf("%8d ", sp->st_size);

	// print time
	strcpy(ftime, ctime(&sp->st_ctime));
	ftime[strlen(ftime) - 1] = 0;
	printf("%s  ", ftime);

	// print name
	printf("%s", basename(fname));

	// print -> linkname if it's a symbolic file
	if ((sp->st_mode & 0xF000) == 0xA000) {
		// use readlink() SYSCALL to read the linkname
		if ((len = readlink(fname, nameBuf, sizeof(nameBuf) - 1)) != -1)
			nameBuf[len] = '\0';
		//printf(" -> %s", linkname);
		printf(" -> %s", nameBuf);
	}
	printf("\n");
}

int ls_dir(char *dname) {
	DIR *dirp;
	struct dirent *dp;

	dirp = opendir(dname); // open the dir
	if (dirp == NULL) {
		printf("open dir %s was failed.\n", dname);
	} else {
		// loop thru every file inside the dir
		while ((dp = readdir(dirp)) != NULL) {
			ls_file(dp->d_name); // ls every single file
		}

		closedir(dirp); // close the dir stream
	}
}

void lcd(char *name) {
	if (strcmp(name, "") == 0) { // no dirName input
		chdir(home); // cd to home directory
		printf("lcd  %s ok.\n", home);
	} else { // has dir argument
		if (chdir(name) == 0)
			printf("lcd  %s ok.\n", name);
		else
			printf("lcd  %s failed.\n", name);
	}
}

void lpwd() {
	char cwd[128];
	getcwd(cwd, 128); // get CWD pathname
	if (cwd != NULL)
		printf("pwd=%s\n", cwd);
	else
		printf("lpwd failed\n");
}

void lmkdir(char *name) {
	r = mkdir(name, 0755); // mkdir("name", mode)
	if (r == 0)
		printf("lmkdir %s ok.\n", name);
	else
		printf("lmkdir %s failed.\n", name);
}

void lrmdir(char *name) {
	r = rmdir(name);
	if (r == 0)
		printf("lrmdir %s ok.\n", name);
	else
		printf("lrmdir %s failed.\n", name);

}

void lrm(char *name) {
	r = unlink(name);
	if (r == 0)
		printf("lrm %s ok.\n", name);
	else
		printf("lrm %s failed.\n", name);
}

