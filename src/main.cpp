#include <iostream>
#include "../inc/refresh_counter.h"
#include "../inc/mem_map.h"
#include "../inc/main.h"

int main(int argc, char **argv)
{
	RefreshCounter ddr3_refresh;
	for(int i = 0; i < MEM_MAP_SEL_NUM; i++)
		std::cout << "(BC, BI): " 
			  << (int) MemConfig[i].BC << ", "
		          << (int) MemConfig[i].BI << std::endl;


	return 0;
}
