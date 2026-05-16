#ifndef __GPT_H
#define __GPT_H

#include "types.h"

typedef struct {
    // 0x0
    u64 guid1;
    u64 guid2;

    // 0x10
    u64 guid3;
    u64 guid4;

    // 0x20
    u64 firstLBA;
    u64 lastLBA;

    // 0x30
    u64 attribFlags;
    char16_t partitionName[0x24];
} gpt_partition_s;

typedef struct {
    // 0x0
    u64 signature;
    u32 rev;
    u32 headerSize;

    // 0x10
    u32 crcHeader;
    u32 reserved;
    u64 currentLBA;

    // 0x20
    u64 backupLBA;
    u64 firstUsableLBA;

    // 0x30
    u64 lastUsableLBA;
    u64 diskGUID1;

    // 0x40
    u64 diskGUID2;
    u64 partitionEntriesLBA;

    // 0x50
    u32 numPartitions;
    u32 partitionEntrySize;
    u32 crcPartitions;
} gpt_s;

#endif
