#include "mft.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mftutil.h"
#include "util.h"

u8 *goToNextAttrib(u8 *curr) {    
    // Are we seeking to the first attribute?
    if (!strncmp(curr, "FILE", 4)) {
        // Find the first attribute offset
        mft_s *mftStruct = (mft_s *)curr;
        return curr + mftStruct->firstAttribOffset;
    }

    // Are there more attributes?
    if (*(u32 *)curr == 0xFFFFFFFF) {
        // No more attributes
        return NULL;
    }

    // This is an attribute. Seek to the next attribute
    attrib_s *attr = (attrib_s *)curr;
    return curr + attr->length;
}

u8 *getDataPtr(u8 *curr) {
    attrib_s *attr = (attrib_s *)curr;
    if (attr->nameLength) {
        // Ignore named attributes for now...
        return NULL;
    }

    if (attr->nonResident) {
        // Ignore non-resident attributes for now...
        return NULL;
    }

    return curr + 0x18;
}

void walkAttributes(mft_s *mftEntry, file_s *fileStruct) {
    memset(fileStruct, 0, sizeof(file_s));
    u8 *currAttrib = (u8 *)mftEntry;
    u8 foundName = 0;
    u8 usedName = 0;
    while (currAttrib = goToNextAttrib(currAttrib)) {
        attrib_s *attr = (attrib_s *)currAttrib;

        if (attr->attribType == 0x30) {
            // File name attribute
            foundName = 1;
            filename_attr_s *fname = (filename_attr_s *)getDataPtr(currAttrib);
            if (!fname) {
                // Couldn't get the data for the file name attribute for some reason...
                continue;
            }

            fileStruct->parent = fname->parentDirectoryMFTEntry & 0xFFFFFFFFFFFF;
            /*
            if (fileStruct->filenames[fname->filenameNamespace]) {
                fprintf(stderr, "MFT entry %d has multiple filename attributes with the same namespace!\n", mftEntry->mftEntryNum);
                exit(EXIT_FAILURE);
            }*/

            fileStruct->filenames[fname->filenameNamespace] = fname;
            usedName = 1;
        }
        else if (attr->attribType == 0x80) {
            // Data attribute
            // ONLY GET THE SIZE FROM THE LAST NON-NAME DATA ATTRIB
            if (attr->nameLength) {
                continue;
            }

            if (attr->nonResident) {
                fileStruct->fileSize = *(u64 *)(currAttrib + 0x30);
            } else {
                fileStruct->fileSize = *(u64 *)(currAttrib + 0x10);
            }
        }
    }

    if (foundName && !usedName) {
        fprintf(stderr, "=== NO PROPER NAME FOUND! ===\n");
        exit(EXIT_FAILURE);
    }
}

u64 getNumMFTEntries() {
    mft_s mft;
    readMFTEntryAt(&mft, 0);

    file_s fileStruct;
    walkAttributes(&mft, &fileStruct);

    u64 numEntries = fileStruct.fileSize / MFT_SIZE;
    printf("The MFT is %llu bytes long. There are %llu MFT entries.\n", fileStruct.fileSize, numEntries);
    return numEntries;
}

filename_attr_s *pickFileName(file_s *fileStruct) {
    if (fileStruct->filenames[3]) // WIN32 & DOS
        return fileStruct->filenames[3];
    else if (fileStruct->filenames[1]) // WIN32
        return fileStruct->filenames[1];
    else if (fileStruct->filenames[0]) // POSIX
        return fileStruct->filenames[0];
    else
        return fileStruct->filenames[2]; // either DOS or NULL
}

u8 printMFTEntry(u64 mftEntryNum, TreeNode *tree) {
    mft_s mftEntry;
    readMFTEntryAt(&mftEntry, mftEntryNum);
    if (!verifyMFTEntry(&mftEntry)) {
        return 0;
    }

    file_s fileStruct;
    walkAttributes(&mftEntry, &fileStruct);
    filename_attr_s *filename = pickFileName(&fileStruct);

    if (!filename && !mftEntry.baseMFTEntry) {
        printf("%llu has no name!\n", mftEntryNum);
        return 1;
    } else if (!filename) {
        // Excusable because this is an "extra" MFT entry
        return 1;
    }

    char16_t *name = (char16_t *)malloc(filename->filenameLength * 2 + 2);
    memcpy(name, (u8 *)filename + 0x42, filename->filenameLength * 2);
    name[filename->filenameLength] = '\0';

    tree[mftEntryNum].name = name;
    InsertNode(tree, fileStruct.parent, mftEntryNum);
    // printf("%llu: ", mftEntryNum);
    // printUTF16(name);
    // putchar('\n');
    // free(name);

    return 1;
}

void printMFTEntries() {
    u64 numEntries = getNumMFTEntries();
    TreeNode *tree = PrepareTree((u32)numEntries);

    for (u64 i = 0; i < numEntries; i++) {
        printMFTEntry(i, tree);
    }

    PrintTree(tree);
    DeleteTree(tree, (u32)numEntries);
}
