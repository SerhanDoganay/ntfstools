#include "util.h"
#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void printUTF16(char16_t *src) {
    setlocale(LC_ALL, "");
    char buf[2048]; // Should be big enough?
    char *out = buf;
    mbstate_t state = {0};

    while (*src) {
        size_t rc = c16rtomb(out, *src++, &state);
        if (rc == (size_t)-1) break;
        out += rc;
    }
    *out = '\0';
    printf("%s", buf);
}

void fprintUTF16(char16_t *src, FILE *fptr) {
    setlocale(LC_ALL, "");
    char buf[2048]; // Should be big enough?
    char *out = buf;
    mbstate_t state = {0};

    while (*src) {
        size_t rc = c16rtomb(out, *src++, &state);
        if (rc == (size_t)-1) break;
        out += rc;
    }
    *out = '\0';
    fprintf(fptr, "%s", buf);
}

void printGUID(u64 guid1, u64 guid2) {
    u32 first = guid1 & 0xFFFFFFFF;
    printf("%08X-", first);

    u16 second = (guid1 & 0xFFFF00000000) >> 32;
    printf("%04X-", second);

    u16 third = (guid1 & 0xFFFF000000000000) >> 48;
    printf("%04X-", third);

    u16 fourth = guid2 & 0xFFFF;
    u64 f1 = fourth & 0xFF;
    u64 f2 = (fourth & 0xFF00) >> 8;

    fourth = (f1 << 8) | f2;
    printf("%04X-", fourth);

    u64 fifth = (guid2 & 0xFFFFFFFFFFFF0000) >> 16;
    f1 = fifth & 0xFF;
    f2 = (fifth & 0xFF00) >> 8;
    u64 f3 = (fifth & 0xFF0000) >> 16;
    u64 f4 = (fifth & 0xFF000000) >> 24;
    u64 f5 = (fifth & 0xFF00000000) >> 32;
    u64 f6 = (fifth & 0xFF0000000000) >> 40;

    fifth = (f1 << 40) | (f2 << 32) | (f3 << 24) | (f4 << 16) | (f5 << 8) | f6;
    printf("%012llX", fifth);
}

void goToSector(u64 sector) {
    if (lseek64(fd, sector * SECTOR_SIZE, SEEK_SET) == -1) {
        char errmsg[256];
        sprintf(errmsg, "Unable to seek to sector %llu", sector);
        perror(errmsg);
        exit(EXIT_FAILURE);
    }
}

void readSector(u8 *buffer) {
    if (read(fd, buffer, SECTOR_SIZE) == -1) {
        perror("Unable to read sector");
        exit(EXIT_FAILURE);
    }
}

void readSectorAt(u8 *buffer, u64 sector) {
    goToSector(sector);
    readSector(buffer);
}

TreeNode *PrepareTree(u32 numNodes) {
    TreeNode *ret = (TreeNode *)malloc(numNodes * sizeof(TreeNode));
    for (u32 i = 0; i < numNodes; i++) {
        // ret[i].parent = NULL;
        ret[i].children = NULL;
        ret[i].name = NULL;
        ret[i].mftEntry = i;
    }
    return ret;
}

void InsertChild(TreeNode *parentNode, TreeNode *childNode) {
    NodeList *newNode = (NodeList *)malloc(sizeof(NodeList));
    newNode->self = childNode;
    newNode->next = parentNode->children;
    parentNode->children = newNode;    
}

void InsertNode(TreeNode *tree, u32 parentID, u32 childID) {
    TreeNode *parentNode = &tree[parentID];
    TreeNode *childNode = &tree[childID];

    if (childID == 5) {
        // Uh, don't create a loop (don't let the root directory set itself as its own parent)
        return;
    }

    // childNode->parent = parentNode;
    InsertChild(parentNode, childNode);
}

void padfile(u32 depth, FILE *fptr) {
    for (u32 i = 0; i < depth; i++) {
        fprintf(fptr, "\t");
    }
}

void padtext(u32 depth, FILE *fptr) {
    for (u32 i = 0; i < depth; i++) {
        fprintf(fptr, "&nbsp;&nbsp;&nbsp;&nbsp;");
    }
}

void PrintNode(TreeNode *node, u32 depth, FILE *fptr) {
    u8 isDir = node->children != 0;

    padfile(depth, fptr);
    if (isDir) {
        fprintf(fptr, "<details><summary>");
    } else {
        fprintf(fptr, "<p>");
    }

    padtext(depth, fptr);
    fprintf(fptr, "[%d] ", node->mftEntry);
    fprintUTF16(node->name, fptr);
    
    if (isDir) {
        fprintf(fptr, "</summary>\n");
    } else {
        fprintf(fptr, "</p>\n");
    }

    // printf("%d : ", node->mftEntry);
    // printUTF16(node->name);
    // putchar('\n');

    NodeList *child = node->children;
    while (child) {
        PrintNode(child->self, depth + 1, fptr);
        child = child->next;
    }

    if (isDir) {
        padfile(depth, fptr);
        fprintf(fptr, "</details>\n");
    }
}

void PrintTree(TreeNode *tree) {
    FILE *fptr;
    fptr = fopen("output.html", "w");
    if (!fptr) {
        fprintf(stderr, "Failed to open html file!\n");
        exit(EXIT_FAILURE);
    }

    fprintf(fptr, "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n\t<meta charset=\"UTF-8\">\n\t<title>NTFS Files and Directories</title>\n</head>\n<body>\n\t<h1>NTFS Files and Directories</h1>\n");
    PrintNode(&tree[5], 0, fptr); // Start with the root dir
    fprintf(fptr, "</body>\n</html>");
    fclose(fptr);
}

void DeleteTree(TreeNode *tree, u32 numNodes) {
    for (u32 i = 0; i < numNodes; i++) {
        TreeNode *curr = &tree[i];

        free(curr->name);
        while (curr->children) {
            NodeList *child = curr->children;
            NodeList *prev = NULL;
            while (child->next) {
                prev = child;
                child = child->next;
            }

            free(child);
            if (prev) {
                prev->next = NULL;
            } else {
                curr->children = NULL;
            }
        }
    }

    free(tree);
}
