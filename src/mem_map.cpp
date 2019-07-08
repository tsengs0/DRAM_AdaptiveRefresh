#include "../inc/mem_map.h"

// Four memory mapping selection:
// ------------------------------
// (BC, BI) | Request Size (Byte)
// ------------------------------
//  1, 1    | 32
//  1, 2    | 64
//  1, 4    | 128
//  2, 4    | 256
struct MemMap MemConfig[MEM_MAP_SEL_NUM] = {
//     (BC, BI)
	{1,1}, // for 32B  
	{1,2}, // for 64B
	{1,4}, // for 128B
	{2,4}  // for 256B
};
