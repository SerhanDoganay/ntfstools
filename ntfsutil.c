#include "ntfsutil.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "util.h"

void goToCluster(u64 cluster) {
    if (lseek64(fd, partitionStart + cluster * CLUSTER_SIZE, SEEK_SET) == -1) {
        char errmsg[256];
        sprintf(errmsg, "Unable to seek to cluster %llu", cluster);
        perror(errmsg);
        exit(EXIT_FAILURE);
    }
}

void readCluster(u8 *buffer) {
    if (read(fd, buffer, CLUSTER_SIZE) == -1) {
        perror("Unable to read cluster");
        exit(EXIT_FAILURE);
    }
}

void readClusterAt(u8 *buffer, u64 cluster) {
    goToCluster(cluster);
    readCluster(buffer);
}
