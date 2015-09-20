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
#include <stddef.h>
struct Node {
	char name[64];
	char type;
	struct Node * parent;
	struct Node * child;
	struct Node * olderSibling;
	struct Node * youngerSibling;
};
typedef struct Node Node;

Node * Node_(char* name, char type, Node * parent, Node * child,
		Node * olderSibling, Node * youngerSibling) {
//	printf("Node_: %s/%s\n", parent->name, name);
	Node * node = malloc(sizeof(Node));
	assert(node != NULL);
	strcpy(node->name, name);
	node->type = type;
	node->parent = parent;
	node->child = child;
	node->olderSibling = olderSibling;
	node->youngerSibling = youngerSibling;
	return node;
}

Node * initialize(void) {
	return Node_("", 'D', NULL, NULL, NULL, NULL);
}

void printChildren(Node * parent) {
	if (parent->type == 'F') {
		printf("%c %s\n", parent->type, parent->name);
	}
	Node * child = parent->child;
	for(; child != NULL; child = child->olderSibling) {
		printf("%c %s\n", child->type, child->name);
	}
}

Node * findChildNode(Node * parent, char* childName) {
	assert(parent != NULL);
//	printf("findChildNode: looking for %s/%s", parent->name, childName);
	if (childName == NULL) {
		return parent;
	}
	Node * candidate;
	if (strcmp(childName, "..") == 0) {
		candidate = parent->parent;
	} else if (strcmp(childName, ".") == 0) {
		candidate = parent;
	} else {
		candidate = parent->child;
		for (; candidate != NULL; candidate = candidate->olderSibling) {
			if (strcmp(candidate->name, childName) == 0)
				break;
		}
	}
//	printf("findChildNode: found %s\n", candidate->name);
	return candidate;
}

int add(Node * parent, char* name, char type) {
	assert(parent != NULL);
	assert(name != NULL);
	if (strcmp(name, "/") == 0) {
		printf("add: invalid file name\n");
		return -1;
	}
	printf("add: %s\n", name);
	Node * target = findChildNode(parent, name);
	if (target != NULL) {
		printf("add: target exists!\n");
		return -1;
	}
	Node * new_child = Node_(name, type, parent, NULL, parent->child, NULL);
	if (parent->child != NULL) {
		parent->child->youngerSibling = new_child;
	}
	parent->child = new_child;
	return 0;
}

int delete(Node * target) {
	printf("delete: %s\n", target->name);
	if (target == NULL) {
		return -1;
	}
	if (target->olderSibling != NULL) {
		target->olderSibling->youngerSibling = target->youngerSibling;
	}
	if (target->youngerSibling != NULL) {
		target->youngerSibling->olderSibling = target->olderSibling;
	}
	if (target->parent->child == target) {
		target->parent->child = target->olderSibling;
	}
	free(target);
	return 0;
}


Node* findTargetNode(char* pathName, Node* root, Node* cwd) {
//	printf("findTargetNode: looking for pathName = %s\n", pathName);
	char pathNameCopy[128];
	strcpy(pathNameCopy, pathName);
	Node* candidate;
	if (pathNameCopy[0] == '/') {
		candidate = root;
	} else {
		candidate = cwd;
	}
	char* fileName;
	fileName = strtok(pathNameCopy, "/");
	candidate = findChildNode(candidate, fileName);
	while ((fileName = strtok(0, "/"))) {
		if (candidate == NULL) {
			break;
		} else {
			candidate = findChildNode(candidate, fileName);
		}
	}
//	printf("findTargetNode: found targetNode = %s\n", candidate->name);
	return candidate;
}

Node * findParentNode(char* pathName, Node* root, Node* cwd) {
	char pathNameCopy[128];
	strcpy(pathNameCopy, pathName);
	char dirName[128];
	strcpy(dirName, dirname(pathNameCopy));
	Node* candidate = findTargetNode(dirName, root, cwd);
	return candidate;
}

void printDir(Node * node, FILE* stream) {
	assert(node != NULL);
	if (node->parent == NULL) {
		return;
	}
	printDir(node->parent, stream);
	fprintf(stream, "/%s", node->name);
	return;
}
void pwd(Node * node, FILE* stream) {
	printDir(node, stream);
	fprintf(stream, "\n");
	return;
}

void traversal(Node * node, FILE* stream) {
	if (node == NULL) {
		return;
	}
	fprintf(stream, "%c ", node->type);
	pwd(node, stream);
	traversal(node->olderSibling, stream);
	traversal(node->child, stream);
}

Node* cd(Node* targetNode, Node* cwd){
	if (targetNode == NULL || targetNode->type != 'D') {
		printf("cd: not a directory!\n");
		return cwd;
	} else {
		printf("cd: success! %s\n", targetNode->name);
		return targetNode;
	}
}

void ls(Node * node){
	printf("ls: %s\n", node->name);
	printChildren(node);
	return;
}


int mkdir(Node * parentNode, char * dirName) {
	if (parentNode == NULL || parentNode->type != 'D') {
		printf("mkdir: unable to find parent directory\n");
		return -1;
	} else {
		int success = add(parentNode, dirName, 'D');
//		printf("mkdir: %s\n", dirName);
		return success;
	}
}

int rmdir(Node* targetNode) {
	if (targetNode == NULL) {
		printf("rmdir: unable to find directory\n");
		return -1;
	}
	printf("rmdir: %s\n", targetNode->name);
	if (targetNode->child != NULL) {
		printf("rmdir: directory not empty!\n");
		return -1;
	} else {
		delete(targetNode);
		return 0;
	}
}

int create(Node * parentNode, char * fileName){
	if (parentNode == NULL) {
		printf("create: unable to find parent directory\n");
		return -1;
	} else {
		int success = add(parentNode, fileName, 'F');
		printf("create: %s\n", fileName);
		return success;
	}
}

int rm(Node* targetNode) {
	if (targetNode == NULL) {
		printf("rm: unable to find file\n");
		return -1;
	}
	printf("rm: %s\n", targetNode->name);
	if (targetNode->type != 'F') {
		printf("rm: not a file!\n");
		return -1;
	} else {
		delete(targetNode);
		return 0;
	}
}

void save(Node* root){
	FILE* drive = fopen("drive", "w+");
	traversal(root, drive);
	fclose(drive);
	return;
}

void quit(){
	exit(0);
}

Node* excecuteCommand(char* command, char* pathName, Node* root, Node* cwd) {
	printf("excecuteCommand: %s %s\n", command, pathName);
	Node * parentNode = findParentNode(pathName, root, cwd);
	Node * targetNode = findTargetNode(pathName, root, cwd);
	char pathNameCopy[128];
	strcpy(pathNameCopy, pathName);
	char baseName[128];
	strcpy(baseName, basename(pathNameCopy));
	if (strcmp(command, "mkdir") == 0) {
		mkdir(parentNode, baseName);
	} else if (strcmp(command, "ls") == 0) {
		if (strcmp(pathName, "") == 0) {
			ls(cwd);
		} else {
			ls(targetNode);
		}
	} else if (strcmp(command, "cd") == 0) {
		if (strcmp(pathName, "") == 0) {
			cwd = cd(root, cwd);
		} else {
			cwd = cd(targetNode, cwd);
		}
	} else if (strcmp(command, "pwd") == 0) {
		pwd(cwd, stdout);
	} else if (strcmp(command, "rmdir") == 0) {
		rmdir(targetNode);
	} else if (strcmp(command, "create") == 0) {
		create(parentNode, baseName);
	} else if (strcmp(command, "rm") == 0) {
		rm(targetNode);
	}
	return cwd;
}

void reload(Node* root, Node* cwd){
//	printf("reload: reloading drive\n");
	FILE* drive = fopen("drive", "r");
	if (drive == NULL) {
		printf("reload: unable to find drive\n");
	}
	char* line = NULL;
	size_t length = 0;
	ssize_t read;
	while((read = getline(&line, &length, drive)) != -1) {
//		printf("reload: creating %s", line);
		char type[1];
		char pathName[128];
		char input[128];
		strcpy(input, line);
		sscanf(input, "%s %s", type, pathName);
//		printf("reload: creating %s %s\n", type, pathName);
		if (pathName[0] != '/') {
			continue;
		} else if (strcmp(type, "D") == 0) {
			excecuteCommand("mkdir", pathName, root, cwd);
		} else if (strcmp(type, "F") == 0) {
			excecuteCommand("create", pathName, root, cwd);
		}
	}
	fclose(drive);
	return;
}

int main2() {
	printf("###############################################################\n");
	Node * root = initialize();
	Node * cwd = root;
//	while(true) {
	int i = 0;
	for (; i<99; i++){
		printf("===========================================================\n");
		printf("print tree: \n");
		traversal(root, stdout);
		printf("print cwd: ");
		pwd(cwd, stdout);
		printf("$ ");
		char input[128];
		fgets(input, 128, stdin);
		char command[128] = "";
		char pathName[128] = "";
		sscanf(input, "%s %s", command, pathName);
		if (strcmp(command, "reload") == 0) {
			reload(root, cwd);
		} else if (strcmp(command, "save") == 0) {
			save(root);
		} else if (strcmp(command, "quit") == 0) {
			quit();
		} else {
			cwd = excecuteCommand(command, pathName, root, cwd);
		}
	}
	return 0;
}
