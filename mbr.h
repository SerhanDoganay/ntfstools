#ifndef __MBR_H
#define __MBR_H

#include "types.h"

typedef struct {
    u8 status;
    u8 chsBegin[3];
    u8 type;
    u8 chsEnd[3];
    u32 startSector;
    u32 numSectors;
} mbr_partition_s;

typedef struct __attribute__((packed)){
    char code[440];
    u32 diskSignature;
    u16 nulls;
    mbr_partition_s partitions[4];
    u16 mbrSignature;
} mbr_s;

#endif
