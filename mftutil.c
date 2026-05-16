#include "mftutil.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "types.h"
#include "util.h"

void goToMFTEntry(u64 mftEntry) {
    if (lseek64(fd, partitionStart + mftStart + mftEntry * MFT_SIZE, SEEK_SET) == -1) {
        char errmsg[256];
        sprintf(errmsg, "Unable to seek to MFT entry %llu", mftEntry);
        perror(errmsg);
        exit(EXIT_FAILURE);
    }
}

void readMFTEntry(mft_s *mftStruct) {
    if (read(fd, mftStruct, MFT_SIZE) == -1) {
        perror("Unable to read MFT entry");
        exit(EXIT_FAILURE);
    }

    // FIXUP TIME
    u16 *fixupArr = (u16 *)((u8 *)mftStruct + mftStruct->updateOffset);
    u16 *sector1fixup = (u16 *)((u8 *)mftStruct + SECTOR_SIZE - 2);
    u16 *sector2fixup = (u16 *)((u8 *)mftStruct + SECTOR_SIZE*2 - 2);

    if (*sector1fixup != fixupArr[0]) {
        fprintf(stderr, "Sector 1 failed the fixup! Rolling with it anyway.");
    }
    if (*sector2fixup != fixupArr[0]) {
        fprintf(stderr, "Sector 2 failed the fixup! Rolling with it anyway.");
    }

    *sector1fixup = fixupArr[1];
    *sector2fixup = fixupArr[2];
}

void readMFTEntryAt(mft_s *mftStruct, u64 mftEntry) {
    goToMFTEntry(mftEntry);
    readMFTEntry(mftStruct);
}

u8 verifyMFTEntry(mft_s *mftStruct) {
    if (strncmp(mftStruct->magic, "FILE", 4)) {
        // Maybe good enough
        return 0;
    }
    return 1;
}
