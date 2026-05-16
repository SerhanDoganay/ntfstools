#include "ntfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ntfsutil.h"
#include "mftutil.h"
#include "util.h"

u32 CLUSTER_SIZE = 0;
u64 mftStart = 0;

void handleVBR() {
    goToCluster(0);

    u8 buf[SECTOR_SIZE];
    readSector(buf);

    vbr_s vbr;
    memcpy(&vbr, buf, sizeof(vbr_s));

    if (strncmp(vbr.ntfsMagic, "NTFS    ", 8)) {
        fprintf(stderr, "Invalid NTFS signature detected. Aborting.");
        exit(EXIT_FAILURE);
    }
    puts("NTFS header found");

    CLUSTER_SIZE = vbr.sectorsPerCluster * SECTOR_SIZE;
    mftStart = vbr.mftCluster * CLUSTER_SIZE;

    printf("Cluster size: 0x%X bytes\n", CLUSTER_SIZE);
    printf("MFT is at cluster 0x%llX (byte address 0x%llX)\n", vbr.mftCluster, mftStart);
}
