#include <iostream>
#include "../inc/refresh_counter.h"
#include "../inc/mem_map.h"
#include "../inc/main.h"

double RefreshInterval = (double) (tREFW / PARTITION_NUM);

int main(int argc, char **argv)
{
	
	RefreshCounter ddr3_refresh(RefreshInterval);
	ddr3_refresh.run_RefreshSim();

	return 0;
}
