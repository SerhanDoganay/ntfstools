#ifndef __MFTUTIL_H
#define __MFTUTIL_H

#include "mft.h"

extern u64 mftStart; // byte address to the start of the MFT in the NTFS partition

void goToMFTEntry(u64 mftEntry);
void readMFTEntry(mft_s *mftStruct);
void readMFTEntryAt(mft_s *mftStruct, u64 mftEntry);
u8 verifyMFTEntry(mft_s *mftSturct);

#endif
