#ifndef __DATARUN_H
#define __DATARUN_H

#include "types.h"

typedef struct datarun_s datarun_s;

struct datarun_s {
    u32 startCluster;
    u32 length;
    datarun_s *next;
};

datarun_s *InitializeRuns(u8 *runs);
void DeleteRuns(datarun_s *runs);
u64 GetNthCluster(datarun_s *runs, u64 cluster);

#endif
