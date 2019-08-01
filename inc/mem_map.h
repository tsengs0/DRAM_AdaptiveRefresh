#ifndef __MEM_MAP_H
#define __MEM_MAP_H

typedef unsigned char uint8_t;
#define MAP_1
#ifdef MAP_1
// Five memory mapping selection:
// IW: 16-bit
// Cited from: [ECRTS 2014] Dynamic Command Scheduling ro Real-Time Memory Controllers 
// ------------------------------
// (BC, BI) | Request Size (Byte)
// ------------------------------
//  1, 1    | 16
//  1, 2    | 32
//  1, 4    | 64
//  2, 4    | 128
//  4, 4    | 256
// ------------------------------
	#define MEM_MAP_SEL_NUM 5 // Five memory mapping selection
	enum pattern_index {
		BI_INDEX = 0,
		BC_INDEX = 1
	};
#elif MAP_2
// Four memory mapping selection:
// Cited from: [DATE 2012] DRAM Selection and Configuration for Real-Time Mobile Systems
// ------------------------------
// (BC, BI) | Request Size (Byte)
// ------------------------------
//  1, 1    | 32
//  1, 2    | 64
//  1, 4    | 128
//  2, 4    | 256
// ------------------------------
	#define MEM_MAP_SEL_NUM 5 // Five memory mapping selection
	enum pattern_index {
		BI_INDEX = 0,
		BC_INDEX = 1
	};
#endif

enum MemMap_Selection {
	REQUEST_16B = 1,
	REQUEST_32B,
	REQUEST_64B,
	REQUEST_128B,
	REQUEST_256B
};

typedef struct MemMap_t {
	uint8_t BI;
	uint8_t BC;
	uint8_t start_bank;
} _MemMap;

#endif // __MEM_MAP_H
