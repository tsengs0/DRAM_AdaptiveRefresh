#ifndef __MEM_MAP_H
#define __MEM_MAP_H

// Four memory mapping selection:
// ------------------------------
// (BC, BI) | Request Size (Byte)
// ------------------------------
//  1, 1    | 32
//  1, 2    | 64
//  1, 4    | 128
//  2, 4    | 256
// ------------------------------

#define MEM_MAP_SEL_NUM 4 // Four memory mapping selection
typedef unsigned char uint8_t;

enum MemMap_Selection {
	REQUEST_32B,
	REQUEST_64B,
	REQUEST_128B,
	REQUEST_256B
};

struct MemMap {
	uint8_t BC;
	uint8_t BI;
};

#endif // __MEM_MAP_H
