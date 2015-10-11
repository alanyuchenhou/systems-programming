// This is the echo SERVER server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#define  MAX 256

// Define variables:
struct sockaddr_in server_addr, client_addr, name_addr;
struct hostent *hp;

int sock, newsock; // socket descriptors
int serverPort; // server port number
int r, length, n; // help variables

int server_init(char *name) {
	printf("==================== server init ======================\n");
	// get DOT name and IP address of this host

	printf("1 : get and show server host info\n");
	hp = gethostbyname(name);
	if (hp == 0) {
		printf("unknown host\n");
		exit(1);
	}
	printf("    hostname=%s  IP=%s\n", hp->h_name,
			inet_ntoa(*(long *) hp->h_addr));

	//  create a TCP socket by socket() syscall
	printf("2 : create a socket\n");
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		printf("socket call failed\n");
		exit(2);
	}

	printf("3 : fill server_addr with host IP and PORT# info\n");
	// initialize the server_addr structure
	server_addr.sin_family = AF_INET; // for TCP/IP
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // THIS HOST IP address
	server_addr.sin_port = 0; // let kernel assign port

	printf("4 : bind socket to host info\n");
	// bind syscall: bind the socket to server_addr info
	r = bind(sock, (struct sockaddr *) &server_addr, sizeof(server_addr));
	if (r < 0) {
		printf("bind failed\n");
		exit(3);
	}

	printf("5 : find out Kernel assigned PORT# and show it\n");
	// find out socket port number (assigned by kernel)
	length = sizeof(name_addr);
	r = getsockname(sock, (struct sockaddr *) &name_addr, &length);
	if (r < 0) {
		printf("get socketname error\n");
		exit(4);
	}

	// show port number
	serverPort = ntohs(name_addr.sin_port); // convert to host ushort
	printf("    Port=%d\n", serverPort);

	// listen at port with a max. queue of 5 (waiting clients)
	printf("5 : server is listening ....\n");
	listen(sock, 5);
	printf("===================== init done =======================\n");
	return 0;
}

int ls(char *name) {
	printf("ls: %s\n", name);
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

		printf("ls %s ok\n", fullName);
	}
	return 0;
}

int ls_file(char *fname) {
	printf("ls_file: %s\n", fname);
	char result[MAX];
	char temp[MAX];
	strcpy(result, " ");
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
	if ((sp->st_mode & 0xF000) == 0x8000) {
		strcat(result, "-");
	}
	if ((sp->st_mode & 0xF000) == 0x4000) {
		strcat(result, "d");
	}
	if ((sp->st_mode & 0xF000) == 0xA000) {
		strcat(result, "l");
	}
	for (i = 8; i >= 0; i--) {
		int length = strlen(result);
		if (sp->st_mode & (1 << i))
			result[length] = t1[i];
		else
			result[length] = t2[i];
		result[length + 1] = '\0';
	}
	sprintf(temp, "%4d %4d %4d %8d ", sp->st_nlink, sp->st_gid, sp->st_uid,
			sp->st_size);
	strcat(result, temp);
	strcpy(ftime, ctime(&sp->st_ctime));
	ftime[strlen(ftime) - 1] = 0;
	sprintf(temp, "%s %s", ftime, basename(fname));
	strcat(result, temp);
	// print -> linkname if it's a symbolic file
//	if ((sp->st_mode & 0xF000) == 0xA000) {
//		// use readlink() SYSCALL to read the linkname
//		if ((len = readlink(fname, nameBuf, sizeof(nameBuf) - 1)) != -1)
//			nameBuf[len] = '\0';
//		//printf(" -> %s", linkname);
//		printf(" -> %s", nameBuf);
//	}
	n = write(newsock, result, MAX);
}

int ls_dir(char *dname) {
	printf("ls_dir: %s\n", dname);
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

int get(char* fileName) {
	char buffer[MAX];
	int size;
	FILE* source = fopen(fileName, "r");
	if (source) {
		while ((size = fread(buffer, 1, MAX, source)) > 0) {
			write(newsock, buffer, MAX);
		}
	}
	fclose(source);
}

void serverExecute(char* command) {
	printf("serverExecute: %s\n", command);
	char* arguments[MAX];
	char* token = NULL;
	int index = 0;
	while ((token = strsep(&command, " ")) != NULL) {
		arguments[index] = token;
		index++;
	}
	arguments[index] = NULL;
	int status = -2;
	if (strcmp(arguments[0], "mkdir") == 0) {
		status = mkdir(arguments[1], 0777);
	} else if (strcmp(arguments[0], "rmdir") == 0) {
		status = rmdir(arguments[1]);
	} else if (strcmp(arguments[0], "rm") == 0) {
		status = remove(arguments[1]);
	} else if (strcmp(arguments[0], "cd") == 0) {
		status = chdir(arguments[1]);
	} else if (strcmp(arguments[0], "pwd") == 0) {
		char cwd[MAX];
		if (getcwd(cwd, sizeof(cwd)) != NULL) {
			char result[MAX];
			sprintf(result, "%s\n", cwd);
			n = write(newsock, result, MAX);
			status = 0;
		}
	} else if (strcmp(arguments[0], "ls") == 0) {
		if (arguments[1] == NULL) {
			arguments[1] = ".";
		}
		status = ls(arguments[1]);
		n = write(newsock, "@", MAX);
	} else if (strcmp(arguments[0], "get") == 0) {
		status = get(arguments[1]);
		n = write(newsock, "@", MAX);
	}
	char result[MAX];
	sprintf(result, "status = %d", status);
	n = write(newsock, result, MAX);
}

int main(int argc, char *argv[]) {
	char *hostname;
	char line[MAX];

	if (argc < 2)
		hostname = "localhost";
	else
		hostname = argv[1];

	server_init(hostname);

	// Try to accept a client request
	while (1) {
		printf("server: accepting new connection ....\n");

		// Try to accept a client connection as descriptor newsock
		length = sizeof(client_addr);
		newsock = accept(sock, (struct sockaddr *) &client_addr, &length);
		if (newsock < 0) {
			printf("server: accept error\n");
			exit(1);
		}
		printf("server: accepted a client connection from\n");
		printf("-----------------------------------------------\n");
		printf("        IP=%s  port=%d\n",
				inet_ntoa(client_addr.sin_addr.s_addr),
				ntohs(client_addr.sin_port));
		printf("-----------------------------------------------\n");

		// Processing loop
		while (1) {
			n = read(newsock, line, MAX);
			if (n == 0) {
				printf("server: client died, server loops\n");
				close(newsock);
				break;
			}
			printf("server: read  n=%d bytes; line=[%s]\n", n, line);
			serverExecute(line);
			printf("server: ready for next request\n");
		}
	}
	return 0;
}

