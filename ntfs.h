#ifndef __NTFS_H
#define __NTFS_H

#include "types.h"

typedef struct __attribute__((packed)){
    // 0x0
    char jumpInstruction[3];
    char ntfsMagic[8];
    u16 bytesPerSector;
    u8 sectorsPerCluster;
    u16 reservedSectors;

    // 0x10
    char res1[0x18];
    u64 totalSectors;

    // 0x30
    u64 mftCluster;
    u64 mftMirrCluster;

    // More, but I don't care
} vbr_s;

void handleVBR();

#endif
