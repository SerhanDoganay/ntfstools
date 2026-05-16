#ifndef __MFT_H
#define __MFT_H

#include "types.h"

#define MFT_SIZE 1024

#define IN_USE 0x01
#define DIRECTORY 0x02

typedef struct {
    // 0x0
    char magic[4];
    u16 updateOffset;
    u16 numFixupEntries;
    u64 logFileSequenceNum;

    // 0x10
    u16 sequenceNum;
    u16 numHardLinks;
    u16 firstAttribOffset;
    u16 flags;
    u32 realSize;
    u32 allocatedSize;

    // 0x20
    u64 baseMFTEntry;
    u16 nextAttribID;
    u16 padding;
    u32 mftEntryNum;

    // 0x30
    char dat[MFT_SIZE - 0x30];
} mft_s;

typedef struct {
    u32 attribType;
    u32 length;
    u8 nonResident;
    u8 nameLength;

    // There's more, but this is all the fields I care about
} attrib_s;

typedef struct {
    // 0x0
    u64 parentDirectoryMFTEntry; // files with multiple filename attrs have this as the same
    u64 creationTime;

    // 0x10
    u64 modificationTime; // when the file's contents were changed
    u64 changeTime; // when the MFT entry was changed

    // 0x20
    u64 readTime;
    u64 allocatedSize;

    // 0x30
    u64 realSize;
    u32 flags;
    u32 unk;

    // 0x40
    u8 filenameLength;
    u8 filenameNamespace;

    // The filename!
} filename_attr_s;

typedef struct {
    filename_attr_s *filenames[4];
    u64 fileSize;
    u64 parent;
} file_s;

void printMFTEntries();

#endif
