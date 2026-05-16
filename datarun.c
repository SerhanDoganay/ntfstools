#include "datarun.h"
#include <stdio.h>
#include <stdlib.h>

s32 readBytes(u8 *src, u8 length) {
    if (length > 4) {
        fprintf(stderr, "Can't read a %d-byte integer for data runs.\n", length);
        exit(EXIT_FAILURE);
    }

    s32 ret = 0;
    for (u8 i = 0; i < length; i++) {
        ret |= (*src++) << (i * 8);
    }

    // Sign-extend
    u8 amt = 32 - length*8;
    ret <<= amt;
    ret >>= amt;

    return ret;
}

datarun_s *InitializeRuns(u8 *runs) {
    datarun_s *head = NULL;
    datarun_s *prev = NULL;
    datarun_s *last = NULL;
    u8 header;
    while (header = *runs++) {
        prev = last;
        last = (datarun_s *)malloc(sizeof(datarun_s));
        last->next = NULL;

        if (prev) {
            prev->next = last;
        } else {
            // No one before me? I'm the head!
            head = last;
        }

        u8 startSize = header >> 4;
        u8 lengthSize = header & 0xF;

        last->length = readBytes(runs, lengthSize);
        runs += lengthSize;
        s32 startOffset = readBytes(runs, startSize);
        runs += startSize;

        if (prev) {
            last->startCluster = (u32)(prev->startCluster + startOffset);
        } else {
            last->startCluster = (u32)startOffset;
        }
    }

    return head;
}

void DeleteRuns(datarun_s *runs) {
    if (!runs) {
        return;
    }

    while (runs->next) {
        datarun_s *curr = runs->next;
        datarun_s *prev = NULL;
        while (curr->next) {
            prev = curr;
            curr = curr->next;
        }

        free(curr);
        if (prev) {
            prev->next = NULL;
        } else {
            runs->next = NULL;
        }
    }
    free(runs);
}

u64 GetNthCluster(datarun_s *runs, u64 cluster) {
    // Converts file-relative cluster numbers to cluster numbers from the start of the partition

    if (!runs) {
        fprintf(stderr, "Can't read from empty data runs!\n");
        exit(EXIT_FAILURE);
    }
    
    u64 oldCluster = cluster;
    datarun_s *base = runs;

    while (base && cluster >= base->length) {
        cluster -= base->length;
        base = base->next;
    }

    if (!base) {
        fprintf(stderr, "Cluster %llu is out of range!\n", oldCluster);
        exit(EXIT_FAILURE);
    }

    return base->startCluster + cluster;
}
