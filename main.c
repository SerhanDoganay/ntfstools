#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mft.h"
#include "ntfs.h"
#include "partitiontable.h"
#include "util.h"

int fd = -1; // fd for the drive we're analyzing

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <drive name>\n", argv[0]);
        return EXIT_FAILURE;
    }

    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("Could not open drive");
        return EXIT_FAILURE;
    }

    findPartitionStart();
    handleVBR();
    printMFTEntries();

    if (close(fd) == -1) {
        perror("Could not close drive");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
