#include "../inc/mem_map.h"

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
	uint8_t bi_bc_pattern[MEM_MAP_SEL_NUM][2] = {
		{1, 1}, // 16-byte
		{2, 1}, // 32-byte
		{4, 1}, // 64-byte
		{4, 2}, // 128-byte
		{4, 4}  // 256-byte	
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
	uint8_t bi_bc_pattern[MEM_MAP_SEL_NUM][2] = {
		{1, 1}, // 16-byte (dummy even if it is undefined in the paper)
		{1, 1}, // 32-byte
		{2, 1}, // 64-byte
		{4, 1}, // 128-byte
		{4, 2}  // 256-byte	
	};
#endif
