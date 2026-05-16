#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gpt.h"
#include "mbr.h"
#include "partitiontable.h"
#include "types.h"
#include "util.h"

u64 partitionStart = 0; // byte address to the start of the partition we're analyzing

void handleGPT(u8 *sector) {
    puts("GPT partition table detected");

    gpt_s gptData;
    memcpy(&gptData, sector, sizeof(gpt_s));

    if (gptData.partitionEntrySize != 0x80) {
        fprintf(stderr, "Each partition entry spans %d bytes (I expected 128). Aborting.\n", gptData.partitionEntrySize);
        exit(EXIT_FAILURE);
    }

    readSectorAt(sector, gptData.partitionEntriesLBA);
    for (int i = 0, offset = 0; i < gptData.numPartitions; i++, offset += 0x80) {
        gpt_partition_s part;

        if (offset >= SECTOR_SIZE) {
            offset = 0;
            readSector(sector);
        }

        memcpy(&part, sector + offset, sizeof(gpt_partition_s));

        if (part.guid1 == 0) {
            // We're probably done
            break;
        }

        printf("=== PARTITION #%d ===\n", i);
        printf("Partition type GUID: ");
        printGUID(part.guid1, part.guid2);
        printf("\nUnique partition GUID: ");
        printGUID(part.guid3, part.guid4);
        putchar('\n');
        printf("Partition name: ");
        printUTF16(&part.partitionName[0]);
        printf("\nPartiton starts at sector 0x%llx\n\n", part.firstLBA);

        if (part.guid1 == 0x4433B9E5EBD0A0A2 && part.guid2 == 0xC79926B7B668C087) {
            printf("Main Windows partition detected! Selected as default partition.\n\n");
            partitionStart = part.firstLBA * SECTOR_SIZE;
        }
    }
}

void handleMBR(u8 *sector) {
    puts("MBR partition table detected");
    fprintf(stderr, "MBR not implemented yet. Aborting.\n");
    exit(EXIT_FAILURE);
}

void findPartitionStart() {
    // MBR or GPT?
    u8 buf[SECTOR_SIZE];
    readSectorAt(buf, 1);

    if (strncmp(buf, "EFI PART", 8) == 0) {
        // GPT!
        handleGPT(buf);
    } else {
        // MBR!
        readSectorAt(buf, 0);
        handleMBR(buf);
    }

    if (!partitionStart) {
        fprintf(stderr, "No default partition selected. Aborting.\n");
        exit(EXIT_FAILURE);
    }
}
