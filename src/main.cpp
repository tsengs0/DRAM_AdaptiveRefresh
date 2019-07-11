#include <iostream>
#include "../inc/refresh_counter.h"
#include "../inc/mem_map.h"
#include "../inc/main.h"

_SysTick_unit RefreshInterval = (_SysTick_unit) (tREFW / PARTITION_NUM);

int main(int argc, char **argv)
{
	if(argc != 2) {
		std::cout << "Please give the filename of access patterns" << std::endl;
		exit(1);
	}
	RefreshCounter ddr3_refresh(RefreshInterval, argv[1]);
	ddr3_refresh.run_RefreshSim();

	// Show experimental result
	ddr3_refresh.showEval(1);
	return 0;
}
