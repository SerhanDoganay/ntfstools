#ifndef __NTFSUTIL_H
#define __NTFSUTIL_H

#include "types.h"

extern u32 CLUSTER_SIZE; // number of bytes per NTFS cluster

void goToCluster(u64 cluster);
void readCluster(u8 *buffer);
void readClusterAt(u8 *buffer, u64 cluster);

#endif
