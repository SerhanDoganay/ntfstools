#ifndef __UTIL_H
#define __UTIL_H

#include "types.h"

#define SECTOR_SIZE 512

extern int fd; // fd for the drive we're analyzing
extern u64 partitionStart; // byte address to the start of the partition we're analyzing

void printUTF16(char16_t *src);
void printGUID(u64 guid1, u64 guid2);

void goToSector(u64 sector);
void readSector(u8 *buffer);
void readSectorAt(u8 *buffer, u64 sector);

typedef struct TreeNode TreeNode;
typedef struct NodeList NodeList;

struct NodeList {
    TreeNode *self;
    NodeList *next;
};

struct TreeNode {
    // TreeNode *parent;
    NodeList *children;
    char16_t *name;
    u32 mftEntry;
};

TreeNode *PrepareTree(u32 numNodes);
void InsertNode(TreeNode *tree, u32 parentID, u32 childID);
void PrintTree(TreeNode *tree);
void DeleteTree(TreeNode *tree, u32 numNodes);

#endif
