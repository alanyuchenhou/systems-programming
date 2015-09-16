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
	printf("Node_: %s/%s\n", parent->name, name);
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
	Node * child = parent->child;
	for(; child != NULL; child = child->olderSibling) {
		printf("%s", child->name);
	}
}

Node * findChildNode(Node * parent, char* childName, char type){
	assert(parent != NULL);
	printf("findChildNode: looking for parent->name = %s, childName = %s, type = %c\n", parent->name, childName, type);
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
			if (strcmp(candidate->name, childName) == 0 && candidate->type == type)
				break;
		}
	}
	printf("findChildNode: found %s\n", candidate->name);
	return candidate;
}

int add(Node * parent, char* name, char type) {
	printf("add: %s\n", name);
	Node * target = findChildNode(parent, name, type);
	if (target != NULL) {
		return -1;
	}
	Node * new_child = Node_(name, type, parent, NULL, parent->child, NULL);
	if (parent->child != NULL) {
		parent->child->youngerSibling = new_child;
	}
	parent->child = new_child;
	return 0;
}

int delete(Node * parent, char* name, char type) {
	Node * target = findChildNode(parent, name, type);
	if (target == NULL) {
		return -1;
	}
	if (target->olderSibling != NULL) {
		target->olderSibling->youngerSibling = target->youngerSibling;
	}
	if (target->youngerSibling != NULL) {
		target->youngerSibling->olderSibling = target->olderSibling;
	}
	free(target);
	return 0;
}

void pwd(Node * node) {
	assert(node != NULL);
	if (node->parent == NULL) {
		return;
	}
	pwd(node->parent);
	printf("/%s", node->name);
	return;
}

int mkdir(Node * targetDir, char * name) {
	assert(targetDir != NULL);
	printf("mkdir: %s\n", name);
	int success = add(targetDir, name, 'D');
	return success;
}

void rmdir(char * cwd, char * pathname) {
	return;
}

void cd(char * cwd, char * pathname){
	return;
}

void ls(Node * node){
	printChildren(node);
	return;
}

void creat(char * cwd, char * pathname){
	return;
}

void rm(char * cwd, char * pathname){
	return;
}

void save(char * cwd, char * pathname){
	return;
}

void reload(char * cwd, char * pathname){
	return;
}

void quit(){
	return;
}

Node * findTargetDir(Node* root, Node* cwd, char* pathName) {
	char pathNameCopy[128];
	strcpy(pathNameCopy, pathName);
	char dirName[128];
	strcpy(dirName, dirname(pathNameCopy));
	printf("findTargetDir: looking for dirName = %s\n", dirName);
	Node * candidate;
	if (dirName[0] == '/') {
		candidate = root;
	} else {
		candidate = cwd;
	}
	char * fileName;
	fileName = strtok(dirName, "/");
	candidate = findChildNode(candidate, fileName, 'D');
	while ((fileName = strtok(0, "/"))) {
		if (candidate == NULL) {
			break;
		} else {
			candidate = findChildNode(candidate, fileName, 'D');
		}
	}
	printf("findTargetDir: found targetNode = %s\n", candidate->name);
	return candidate;
}

void excecuteCommand(char* pathName, char* command, Node* targetNode, Node* cwd) {
	printf("excecuteCommand: %s %s\n", command, pathName);
	char pathNameCopy[128];
	strcpy(pathNameCopy, pathName);
	char baseName[128];
	strcpy(baseName, basename(pathNameCopy));
	printf("excecuteCommand: baseName = %s \n", baseName);
	if (strcmp(command, "mkdir") == 0) {
		mkdir(targetNode, baseName);
	} else if (strcmp(command, "ls") == 0) {
		ls(cwd);
	}
}

void traversal(Node * node) {
	if (node == NULL) {
		return;
	}
	printf("%c ", node->type);
	pwd(node);
	printf("\n");
	traversal(node->olderSibling);
	traversal(node->child);
}

int main() {
	Node * root = initialize();
	Node * cwd = root;
//	while(true) {
	int i = 0;
	for (; i<99; i++){
		printf("##########################################################\n");
		printf("print tree: \n");
		traversal(root);
		pwd(cwd);
		printf("$ ");
		char input[128];
		fgets(input, 128, stdin);
		char command[128];
		char pathName[128];
		sscanf(input, "%s %s", command, pathName);
		Node * targetNode = findTargetDir(root, cwd, pathName);
		if (targetNode == NULL) {
			printf("no such file or directory: %s\n", pathName);
			continue;
		}
		excecuteCommand(pathName, command, targetNode, cwd);
	}
	return 0;
}
