/*
 *      Author: Yuchen Hou
 *      ID: 11388981
 */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <ext2fs/ext2_fs.h>
#define BLOCK_SIZE 1024
#define MAX 512
#define INODES_PER_BLOCK 8
typedef struct ext2_super_block Super;
typedef struct ext2_group_desc GroupDesc;
typedef struct ext2_inode Inode;
typedef struct ext2_dir_entry_2 DirEntry;

int printArray(char ** fileNames) {
	for (int index = 0; fileNames[index] != NULL; index++) {
		printf("%s ", fileNames[index]);
	}
	puts("");
	return 0;
}

char ** parsePath(char * path) {
	assert(path != NULL);
	char ** fileNames = malloc(MAX * sizeof(char *));
	char * token = strsep(&path, "/");
	token = strsep(&path, "/");
	for (int index = 0; token != NULL; index++) {
		fileNames[index] = malloc(MAX * sizeof(char));
		fileNames[index] = token;
		fileNames[index + 1] = NULL;
		token = strsep(&path, "/");
	}
	return fileNames;
}

char * getBlock(int fd, int blockNumber) {
	lseek(fd, (long) blockNumber * BLOCK_SIZE, 0);
	char * block = malloc(BLOCK_SIZE);
	assert(block != NULL);
	read(fd, block, BLOCK_SIZE);
	return block;
}

int _get_block(int fd, int blk, char buf[]) {
	lseek(fd, (long) blk * BLOCK_SIZE, 0);
	read(fd, buf, BLOCK_SIZE);
	return 0;
}

int put_block(int fd, int blk, char buf[]) {
	lseek(fd, (long) blk * BLOCK_SIZE, 0);
	write(fd, buf, BLOCK_SIZE);
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

Super * Super_(int fd) {
	Super * super = (Super *) getBlock(fd, 1);
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

GroupDesc * Bgd_(int fd) {
	GroupDesc * groupDescriptor = (GroupDesc *) getBlock(fd, 2);
//	printf("bg_block_bitmap = %d\n", groupDescriptor->bg_block_bitmap);
//	printf("bg_inode_bitmap = %d\n", groupDescriptor->bg_inode_bitmap);
//	printf("bg_inode_table = %d\n", groupDescriptor->bg_inode_table);
//	printf("bg_free_inodes_count = %d\n",
//			groupDescriptor->bg_free_inodes_count);
//	printf("bg_free_blocks_count = %d\n",
//			groupDescriptor->bg_free_blocks_count);
//	printf("bg_used_dirs_count = %d\n", groupDescriptor->bg_used_dirs_count);
	return groupDescriptor;
}

Inode * Inode_(int fd, int inodeTable, int inodeNumber) {
	assert(inodeNumber > 0);
	int blockNumber = inodeTable + (inodeNumber - 1) / INODES_PER_BLOCK;
	char* block = getBlock(fd, blockNumber);
	Inode* inode = (Inode*) block + (inodeNumber - 1) % INODES_PER_BLOCK;
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

int printBitmap(int fd, int bitmapSize, int bitmapBlock) {
	char * blockBitmap = getBlock(fd, bitmapBlock);
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

int decFreeInodes(int fd) {
	char buf[BLOCK_SIZE];
	// dec free inodes count in Super and GroupDesc
	_get_block(fd, 1, buf);
	Super * super = (Super *) buf;
	super->s_free_inodes_count--;
	put_block(fd, 1, buf);
	_get_block(fd, 2, buf);
	GroupDesc * gp = (GroupDesc *) buf;
	gp->bg_free_inodes_count--;
	put_block(fd, 2, buf);
	return 0;
}

int ialloc(int fd) {
	fputs("ialloc: ##################################################", stdout);
	char buf[BLOCK_SIZE];
	_get_block(fd, 1, buf);
	Super * super = (Super *) buf;
	int ninodes = (int) super->s_inodes_count;
	printf("ninodes = %d\n", ninodes);
	_get_block(fd, 2, buf);
	GroupDesc * gp = (GroupDesc *) buf;
	int imap = (int) gp->bg_inode_bitmap;
	printf("imap = %d\n", imap);
	// read inode_bitmap block
	_get_block(fd, imap, buf);
	for (int i = 0; i < ninodes; i++) {
		if (tst_bit(buf, i) == 0) {
			set_bit(buf, i);
			decFreeInodes(fd);
			put_block(fd, imap, buf);
			return i + 1;
		}
	}
	printf("ialloc(): no more free inodes\n");
	return 0;
}

int blockSearch(int fd, int blockNumber, char targetType, char * targetName) {
	assert(blockNumber != 0);
	char * blockHead = getBlock(fd, blockNumber);
	char * blockTail = blockHead + BLOCK_SIZE;
	for (DirEntry * dirEntry = (DirEntry *) blockHead;
			dirEntry < (DirEntry *) blockTail;
			dirEntry = (DirEntry *) ((char *) dirEntry + dirEntry->rec_len)) {
		char actualName[dirEntry->name_len + 1];
		strncpy(actualName, dirEntry->name, dirEntry->name_len);
		actualName[dirEntry->name_len] = '\0';
		if (targetType == dirEntry->file_type
				&& strcmp(targetName, actualName) == 0) {
			return (int) dirEntry->inode;
		}
	}
	return 0;
}

Inode * inodeSearch(int fd, int inodeTable, Inode * inode, char targetType,
		char * targetName) {
	assert(inode != NULL);
	printf("inodeSearch: targetType = %x; targetName = %s;\n", targetType,
			targetName);
	for (int index = 0; inode->i_block[index] != 0; index++) {
		int inodeNumber = blockSearch(fd, (int) inode->i_block[index],
				targetType, targetName);
		if (inodeNumber != 0) {
			return Inode_(fd, inodeTable, inodeNumber);
		}
	}
	return NULL;
}

int printBlocks(int fd, int leftover, int * blockNumbers, int size) {
	for (int index = 0; leftover > 0 && index < size; index++) {
		char * block = getBlock(fd, blockNumbers[index]);
		printf("%.*s", BLOCK_SIZE, block);
		leftover -= BLOCK_SIZE;
	}
	return leftover;
}

int inodeShow(int fd, Inode * inode) {
	assert(inode != NULL);
	puts("inodeShow:");
	if (inode == NULL) {
		return 0;
	}
	int leftover = (int) inode->i_size;
	int blockMax = BLOCK_SIZE / sizeof(int);
	int * blockNumbers = (int *) inode->i_block;
	leftover = printBlocks(fd, leftover, blockNumbers, 12);
	blockNumbers = (int *) getBlock(fd, (int) inode->i_block[12]);
	leftover = printBlocks(fd, leftover, blockNumbers, blockMax);
	int * blockBlockNumbers = (int *) getBlock(fd, (int) inode->i_block[13]);
	for (int index = 0; leftover > 0 && index < blockMax; index++) {
		blockNumbers = (int *) getBlock(fd, blockBlockNumbers[index]);
		leftover = printBlocks(fd, leftover, blockNumbers, blockMax);
	}
	return 0;
}

Inode * groupSearch(int fd, char ** fileNames, GroupDesc * groupDesc) {
	puts("groupSearch:");
	printArray(fileNames);
	Inode * root = Inode_(fd, (int) groupDesc->bg_inode_table, 2);
	Inode * probe = root;
	for (int index = 0; fileNames[index] != NULL; index++) {
		if (fileNames[index + 1] != NULL) {
			probe = inodeSearch(fd, (int) groupDesc->bg_inode_table, probe,
					EXT2_FT_DIR, fileNames[index]);
		} else {
			probe = inodeSearch(fd, (int) groupDesc->bg_inode_table, probe,
					EXT2_FT_REG_FILE, fileNames[index]);
		}
		if (probe == NULL) {
			break;
		}
	}
	return probe;
}

int main(int argc, char * argv[]) {
	assert(argc == 3);
	int fd = open(argv[1], O_RDWR);
	assert(fd >= 0);
	char ** fileNames = parsePath(argv[2]);
//	Super * super = Super_(fd);
	GroupDesc * groupDesc = Bgd_(fd);
//	char buf[BLOCK_SIZE];
//	for (int i = 0; i < 5; i++) {
//		int ino = ialloc(fd);
//		printf("allocated ino = %d\n", ino);
//	}
//	printBitmap(fd, (int) super->s_inodes_count,
//			(int) groupDesc->bg_inode_bitmap);
//	printBitmap(fd, (int) super->s_blocks_count,
//			(int) groupDesc->bg_block_bitmap);
	Inode * target = groupSearch(fd, fileNames, groupDesc);
	if (target == NULL) {
		printf("main: unable to find ");
		printArray(fileNames);
	} else {
		inodeShow(fd, target);
	}
	close(fd);
	return EXIT_SUCCESS;
}
