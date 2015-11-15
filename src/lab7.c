/*
 *      Author: Yuchen Hou
 *      ID: 11388981
 */
#include <assert.h>
#include <dirent.h>
#include <ext2fs/ext2_fs.h>
#include <fcntl.h>
#include <libgen.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define BLOCK_SIZE 1024
#define MAX 512
#define BITS_PER_BLOCK (8*BLOCK_SIZE)
#define INODES_PER_BLOCK 8
#define COMMAND_COUNT 64

// Block number of EXT2 FS on FD
#define SUPERBLOCK        1
#define GDBLOCK           2
#define BBITMAP           3
#define IBITMAP           4
#define INODEBLOCK        5
#define ROOT_INODE        2

// Default dir and regulsr file modes
#define DIR_MODE          0040777
#define FILE_MODE         0100644
#define SUPER_MAGIC       0xEF53
#define SUPER_USER        0

// Proc status
#define FREE              0
#define READY             1
#define RUNNING           2

// Table sizes
#define NMINODES         256
#define NMOUNT            10
#define NPROC             10
#define NFD               10
#define NOFT             100

char * getBlock(int dev, int blockNumber) {
	lseek(dev, (long) blockNumber * BLOCK_SIZE, 0);
	char * block = malloc(BLOCK_SIZE);
	assert(block != NULL);
	read(dev, block, BLOCK_SIZE);
	return block;
}

int _get_block(int dev, int blk, char buf[]) {
	lseek(dev, (long) blk * BLOCK_SIZE, 0);
	read(dev, buf, BLOCK_SIZE);
	return 0;
}

int putBlock(int dev, int blockNumber, char buf[]) {
	lseek(dev, (long) blockNumber * BLOCK_SIZE, 0);
	write(dev, buf, BLOCK_SIZE);
	return 0;
}

int tst_bit(char *buf, int bit) {
	int i, j;
	i = bit / 8;
	j = bit % 8;
	if (buf[i] & (1 << j))
		return 1;
	return 0;
}

int set_bit(char *buf, int bit) {
	int i, j;
	i = bit / 8;
	j = bit % 8;
	buf[i] |= (1 << j);
	return 0;
}

int clr_bit(char *buf, int bit) {
	int i, j;
	i = bit / 8;
	j = bit % 8;
	buf[i] &= ~(1 << j);
	return 0;
}

int printArray(char ** fileNames) {
//	puts("printArray:");
	for (int index = 0; fileNames[index] != NULL; index++) {
		printf("%s ", fileNames[index]);
	}
	puts("");
	return 0;
}

char ** tokenize(char * path) {
	assert(path != NULL);
	char ** fileNames = malloc(MAX * sizeof(char *));
	char * token;
	if (path[0] == '/') {
		token = strsep(&path, "/");
	}
	token = strsep(&path, "/");
	for (int index = 0; token != NULL; index++) {
		fileNames[index] = malloc(MAX * sizeof(char));
		fileNames[index] = token;
		fileNames[index + 1] = NULL;
		token = strsep(&path, "/");
	}
	return fileNames;
}

typedef struct ext2_super_block Super;
Super * Super_(int dev) {
	Super * super = (Super *) getBlock(dev, 1);
	assert(super->s_magic == 0xEF53);
//	printf("s_inodes_count = %d\n", super->s_inodes_count);
//	printf("s_blocks_count = %d\n", super->s_blocks_count);
	//	printf("s_free_inodes_count = %d\n", super->s_free_inodes_count);
	//	printf("s_free_blocks_count = %d\n", super->s_free_blocks_count);
	//	printf("s_first_data_blcok = %d\n", super->s_first_data_block);
	//	printf("s_magic = %x\n", super->s_magic);
	//	printf("s_log_block_size = %d\n", super->s_log_block_size);
	//	printf("s_log_frag_size = %d\n", super->s_log_frag_size);
	//	printf("s_blocks_per_group = %d\n", super->s_blocks_per_group);
	//	printf("s_frags_per_group = %d\n", super->s_frags_per_group);
	//	printf("s_inodes_per_group = %d\n", super->s_inodes_per_group);
	//	printf("s_mnt_count = %d\n", super->s_mnt_count);
	//	printf("s_max_mnt_count = %d\n", super->s_max_mnt_count);
	//	printf("s_inodes_count = %d\n", super->s_inodes_count));
	//	printf("s_wtime = %s", ctime((time_t *) &super->s_wtime));
	return super;
}
typedef struct ext2_group_desc Bgd;
Bgd * Bgd_(int dev) {
	Bgd * bgd = (Bgd *) getBlock(dev, 2);
//	printf("bg_inode_table = %d\n", bgd->bg_inode_table);
//	printf("bg_block_bitmap = %d\n", bgd->bg_block_bitmap);
//	printf("bg_inode_bitmap = %d\n", bgd->bg_inode_bitmap);
//	printf("bg_free_inodes_count = %d\n",
//			bgd->bg_free_inodes_count);
//	printf("bg_free_blocks_count = %d\n",
//			bgd->bg_free_blocks_count);
//	printf("bg_used_dirs_count = %d\n", bgd->bg_used_dirs_count);
	return bgd;
}
typedef struct ext2_inode Inode;
Inode * Inode_(int dev, int inodeTable, int ino) {
	assert(ino > 0);
	int blockNumber = inodeTable + (ino - 1) / INODES_PER_BLOCK;
	char* block = getBlock(dev, blockNumber);
	Inode* inode = (Inode*) block + (ino - 1) % INODES_PER_BLOCK;
//	printf("mode=%4x\n", inode->i_mode);
//	printf("uid=%d\n", inode->i_uid);
//	printf("gid=%d\n", inode->i_gid);
//	printf("i_size=%d\n", inode->i_size);
//	printf("time=%s", ctime((time_t *) &inode->i_mtime));
//	printf("links=%d\n", inode->i_links_count);
//	printf("i_blocks=%d\n", inode->i_blocks);
//	printf("i_block[0]=%d\n", inode->i_block[0]);
	return inode;
}
typedef struct ext2_dir_entry_2 DirEntry;

typedef struct minode {
	Inode * inode; // disk inode
	int dev;
	int ino;
	int refCount;
	int dirty;
	int mounted;
	struct mount * mountptr;
} Minode;

Minode * Minode_(int dev, int inodeTable, int ino, int refCount) {
	Minode * minode = malloc(sizeof(Minode));
	assert(ino > 0);
	minode->inode = Inode_(dev, inodeTable, ino);
	minode->dev = dev;
	minode->ino = ino;
	minode->refCount = refCount;
	minode->dirty = 0;
	minode->mounted = 0;
	minode->mountptr = NULL;
	return minode;
}

Minode ** Minodes_(int dev, int ino) {
	assert(ino != 0);
	int inodeTable = (int) Bgd_(dev)->bg_inode_table;
	Minode ** minodes = malloc(NMINODES * sizeof(Minode *));
	for (int index = 0; index < NMINODES; index++) {
		minodes[index] = Minode_(dev, inodeTable, ino, 0);
	}
	return minodes;
}

typedef struct oft {
	int mode;
	int refCount;
	Minode * minode;
	int offset;
} Oft;

Oft * Oft_(Minode * minode) {
	Oft * oft = malloc(sizeof(Oft));
	oft->minode = minode;
	return oft;
}

typedef struct proc {
	int uid;
	int pid;
	int gid;
	int ppid;
	struct proc * parent;
	int status;
	struct minode * cwd;
//	Oft *dev[NFD];
} Proc;

Proc * Proc_(int uid, Minode * root) {
	Proc * proc = malloc(sizeof(Proc));
	proc->uid = uid;
	proc->cwd = root;
//	proc->dev[0] = Oft_(root);
	return proc;
}

typedef struct mount {
	int ninodes;
	int nblocks;
	int dev;
	int bmap;
	int imap;
	int iblk;
	Minode * mounted_inode;
	char name[256];
	char mount_name[64];
} Mount;

typedef int (*FunctionPointer)(Minode **, Proc *, char *);

int printBitmap(int dev, int bitmapSize, int bitmapBlock) {
	char * blockBitmap = getBlock(dev, bitmapBlock);
	for (int i = 0; i < bitmapSize; i++) {
		if ((i % 8) == 0)
			printf(" ");
		if ((i % 64) == 0)
			printf("\n");
		(tst_bit(blockBitmap, i)) ? putchar('1') : putchar('0');
	}
	printf("\n");
	return 0;
}

int decFreeInodes(int dev) {
	char buf[BLOCK_SIZE];
	// dec free inodes count in Super and Bgd
	_get_block(dev, 1, buf);
	Super * super = (Super *) buf;
	super->s_free_inodes_count--;
	putBlock(dev, 1, buf);
	_get_block(dev, 2, buf);
	Bgd * gp = (Bgd *) buf;
	gp->bg_free_inodes_count--;
	putBlock(dev, 2, buf);
	return 0;
}

int ialloc(int dev) {
	fputs("ialloc: ##################################################", stdout);
	char buf[BLOCK_SIZE];
	_get_block(dev, 1, buf);
	Super * super = (Super *) buf;
	int ninodes = (int) super->s_inodes_count;
	printf("ninodes = %d\n", ninodes);
	_get_block(dev, 2, buf);
	Bgd * gp = (Bgd *) buf;
	int imap = (int) gp->bg_inode_bitmap;
	printf("imap = %d\n", imap);
	// read inode_bitmap block
	_get_block(dev, imap, buf);
	for (int i = 0; i < ninodes; i++) {
		if (tst_bit(buf, i) == 0) {
			set_bit(buf, i);
			decFreeInodes(dev);
			putBlock(dev, imap, buf);
			return i + 1;
		}
	}
	printf("ialloc(): no more free inodes\n");
	return 0;
}

int printBlocks(int dev, int leftover, int * blockNumbers, int size) {
	for (int index = 0; leftover > 0 && index < size; index++) {
		char * block = getBlock(dev, blockNumbers[index]);
		printf("%.*s", BLOCK_SIZE, block);
		leftover -= BLOCK_SIZE;
	}
	return leftover;
}

Minode * getInode(Minode ** minodes, int dev, int inumber) {
	if (inumber == 0) {
		return NULL;
	}
	int inodeTable = (int) Bgd_(dev)->bg_inode_table;
//	printf("getInode: dev = %d; inumber = %d;\n", dev, inumber);
	for (int index = 0; index < NMINODES; index++) {
		Minode * minode = minodes[index];
		if (minode->refCount > 0 && minode->dev == dev
				&& minode->ino == inumber) {
			minode->refCount++;
			return minode;
		}
	}
	Minode* minode = Minode_(dev, inodeTable, inumber, 1);
	for (int index = 0; index < NMINODES; index++) {
		if (minodes[index]->refCount == 0) {
			minodes[index] = minode;
			return minode;
		}
	}
	return NULL;
}

int putInode(Minode * minode) {
	if (minode->dirty == 0) {
		return 0;
	} else {
		int dev = minode->dev;
		int ino = minode->ino;
		int inodeTable = (int) Bgd_(dev)->bg_inode_table;
		int blockNumber = inodeTable + (ino - 1) / INODES_PER_BLOCK;
		char * block = getBlock(dev, blockNumber);
		Inode * inode = (Inode*) block + (ino - 1) % INODES_PER_BLOCK;
		strncpy((char *) inode, (char *) minode->inode, sizeof(Inode));
		putBlock(dev, blockNumber, block);
		return 1;
	}
}

int blockSearch(int dev, int blockNumber, char * targetName) {
	printf("blockSearch: blockNumber = %d; targetName = %s;\n", blockNumber,
			targetName);
	assert(blockNumber != 0);
	char * blockHead = getBlock(dev, blockNumber);
	char * blockTail = blockHead + BLOCK_SIZE;
	for (DirEntry * dirEntry = (DirEntry *) blockHead;
			dirEntry < (DirEntry *) blockTail;
			dirEntry = (DirEntry *) ((char *) dirEntry + dirEntry->rec_len)) {
		char actualName[dirEntry->name_len + 1];
		strncpy(actualName, dirEntry->name, dirEntry->name_len);
		actualName[dirEntry->name_len] = '\0';
		puts(actualName);
		if (strcmp(targetName, actualName) == 0) {
			return (int) dirEntry->inode;
		}
	}
	return 0;
}

int inodeSearch(Minode ** minodes, int dev, int ino, char * targetName) {
	assert(ino != 0);
//	printf("inodeSearch: targetName = %s;\n", targetName);
	Minode * minode = getInode(minodes, dev, ino);
	if (minode == NULL) {
		return 0;
	}
	Inode * inode = minode->inode;
	for (int index = 0; inode->i_block[index] != 0; index++) {
		int ino = blockSearch(dev, (int) inode->i_block[index], targetName);
		if (ino != 0) {
			return ino;
		}
	}
	return 0;
}

//int getInumber(Minode ** minodes, int dev, char * path) {
int getInumber(Minode ** minodes, Proc * running, char * path) {
	int ino = 0;
	if (path == NULL) {
		return ino;
	}
	if (strcmp(path, "/") == 0) {
		return ROOT_INODE;
	}
	int dev = running->cwd->dev;
	if (path[0] == '/') {
		ino = ROOT_INODE;
	} else {
		ino = running->cwd->ino;
	}
	char ** fileNames = tokenize(path);
	printf("getInumber: ");
	printArray(fileNames);
	for (int index = 0; fileNames[index] != NULL; index++) {
		ino = inodeSearch(minodes, dev, ino, fileNames[index]);
		if (ino == 0) {
			break;
		}
	}
	return ino;
}

Minode * findMinode(Minode ** minodes, Proc * running, char * path) {
	int dev = running->cwd->dev;
	int inumber = getInumber(minodes, running, path);
	return getInode(minodes, dev, inumber);
}

int ls(Minode ** minodes, Proc * running, char * path) {
	int dev = running->cwd->dev;
	Minode * target;
	if (strcmp(path, "") == 0) {
		target = running->cwd;
	} else {
		target = findMinode(minodes, running, path);
	}
	puts("ls:----------------------------------------------------------------");
	if (target == NULL) {
		puts("unable to find target file");
		return -1;
	}
	if (S_ISDIR(target->inode->i_mode)) {
		inodeSearch(minodes, dev, target->ino, "");
	} else {
		puts(basename(path));
	}
	return 0;
}

int cat(Minode ** minodes, Proc * running, char * path) {
	Minode * target = findMinode(minodes, running, path);
	if (target == NULL) {
		puts("unable to find target file");
		return -1;
	}
	Inode * inode = target->inode;
	if (S_ISDIR(inode->i_mode)) {
		puts("unable to cat directory!");
		return -1;
	}
	puts("cat:----------------------------------------------------------");
	int dev = running->cwd->dev;
	int blockCount = BLOCK_SIZE / sizeof(int);
	int leftover = (int) inode->i_size;
	int * blockNumbers = (int *) inode->i_block;
	leftover = printBlocks(dev, leftover, blockNumbers, 12);
	blockNumbers = (int *) getBlock(dev, (int) inode->i_block[12]);
	leftover = printBlocks(dev, leftover, blockNumbers, blockCount);
	int * blockBlockNumbers = (int *) getBlock(dev, (int) inode->i_block[13]);
	for (int index = 0; leftover > 0 && index < blockCount; index++) {
		blockNumbers = (int *) getBlock(dev, blockBlockNumbers[index]);
		leftover = printBlocks(dev, leftover, blockNumbers, blockCount);
	}
	return 0;
}

int cd(Minode ** minodes, Proc * running, char * path) {
	if (strcmp(path, "") == 0) {
		running->cwd = getInode(minodes, running->cwd->dev, ROOT_INODE);
		return 0;
	}
	Minode * target = findMinode(minodes, running, path);
	if (target == NULL) {
		puts("unable to find target file");
		return -1;
	}
	if (!S_ISDIR(target->inode->i_mode)) {
		puts("target is not a directory");
		return -1;
	}
	running->cwd = target;
	return 0;
}

int status(Minode ** minodes, Proc * running, char * path) {
	char path1[MAX];
	strncpy(path1, path, MAX);
	Minode * target = findMinode(minodes, running, path);
	if (target == NULL) {
		puts("unable to find target file");
		return -1;
	}
	Inode * inode = target->inode;
	char result[MAX] = "";
	char temp[MAX] = "";
	char *t1 = "xwrxwrxwr-------";
	char *t2 = "----------------";
	if (S_ISDIR(inode->i_mode)) {
		strcat(result, "d");
	} else if (S_ISREG(inode->i_mode)) {
		strcat(result, "-");
	} else if (S_ISLNK(inode->i_mode)) {
		strcat(result, "l");
	}
	for (int i = 8; i >= 0; i--) {
		size_t length = strlen(result);
		if (inode->i_mode & (1 << i))
			result[length] = t1[i];
		else
			result[length] = t2[i];
		result[length + 1] = '\0';
	}
	sprintf(temp, "%4d %4d %4d %8d ", inode->i_links_count, inode->i_gid,
			inode->i_uid, inode->i_size);
	strcat(result, temp);
	time_t timer = (time_t) inode->i_ctime;
	strcpy(temp, ctime(&timer));
	temp[strlen(temp) - 1] = ' ';
	strcat(result, temp);
	strcat(result, basename(path1));
	puts(result);
	return 0;
}

int quit(Minode ** minodes) {
	for (int index = 0; index < NMINODES; index++) {
		putInode(minodes[index]);
	}
	return 0;
}

int runCommand(Minode ** minodes, Proc * running, char * command, char * path) {
	char * commands[COMMAND_COUNT] = { "ls", "cat", "cd", "status" };
	FunctionPointer function[COMMAND_COUNT] = { ls, cat, cd, status };
	for (int commandID = 0; commands[commandID] != NULL; commandID++) {
		if (strcmp(commands[commandID], command) == 0) {
			function[commandID](minodes, running, path);
			return 0;
		}
	}
	printf("runCommand: unable to find command: %s!\n", command);
	return -1;
}

int main(int argc, char * argv[]) {
	assert(argc == 2);
	int dev = open(argv[1], O_RDWR);
	assert(dev >= 0);
	Super_(dev);
	Minode ** minodes = Minodes_(dev, ROOT_INODE);
	Proc * procs[2];
	procs[0] = Proc_(0, getInode(minodes, dev, ROOT_INODE));
	procs[1] = Proc_(1, getInode(minodes, dev, ROOT_INODE));
	Proc * running = procs[1];
//	for (int i = 0; i < 5; i++) {
//		int ino = ialloc(dev);
//		printf("allocated ino = %d\n", ino);
//	}
//	printBitmap(dev, (int) super->s_inodes_count,
//			(int) bgd->bg_inode_bitmap);
//	printBitmap(dev, (int) super->s_blocks_count,
//			(int) bgd->bg_block_bitmap);
	while (1) {
		puts("===============================================================");
		printf("$ ");
		char input[MAX];
		fgets(input, MAX, stdin);
		char command[MAX] = "";
		char path[MAX] = "";
		sscanf(input, "%s %s", command, path);
		if (strcmp(command, "quit") == 0) {
			quit(minodes);
			break;
		} else {
			runCommand(minodes, running, command, path);
		}
	}
	close(dev);
	puts("--------------------------------------------------------------");
	return EXIT_SUCCESS;
}
