#include <iostream>
#include "../inc/refresh_counter.h"
#include "../inc/mem_map.h"
#include "../inc/main.h"

double RefreshInterval = (double) (tREFW / PARTITION_NUM);

int main(int argc, char **argv)
{
	if(argc != 2) {
		std::cout << "Please give the filename of access patterns" << std::endl;
		exit(1);
	}
	RefreshCounter ddr3_refresh(RefreshInterval, argv[1]);
	ddr3_refresh.run_RefreshSim();

	return 0;
}
