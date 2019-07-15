#include <iostream>
#include "../inc/refresh_counter.h"
#include "../inc/mem_map.h"
#include "../inc/main.h"

_SysTick_unit round_length;
_SysTick_unit RefreshInterval_1 = (_SysTick_unit) (tREFW / SUB_WINDOW_NUM);
_SysTick_unit RefreshInterval_2 = (_SysTick_unit) (tREFW / PARTITION_NUM);

void sys_init(char *pattern_filename);
void sim_approach_1(void);
void sim_approach_2(void);

AccessRefreshCounter *ddr3_refresh_1;
RefreshCounter *ddr3_refresh_2;

int main(int argc, char **argv)
{
	if(argc != 3) {
		std::cout << "Please give the filename of access patterns and the approach you want to simulate (by numberring)" << std::endl;
		exit(1);
	}
	sys_init(argv[1]);

	switch(argv[2][0]) {
		case '1':
			sim_approach_1();
			break;
		case '2':
			sim_approach_2();
			break;
		default:
			sim_approach_1(); 
			std::cout << "###########################################" << std::endl;
			sim_approach_2();
	}

	return 0;
}

void sys_init(char *pattern_filename)
{
	round_length = (_SysTick_unit) tREFW;	

	ddr3_refresh_1 = new AccessRefreshCounter(RefreshInterval_1, pattern_filename);
	ddr3_refresh_2 = new RefreshCounter(RefreshInterval_2, pattern_filename);

}

void sim_approach_1(void)
{
	std::cout << "Running simulation: access-aware refresh counter" << std::endl << std::endl;
	ddr3_refresh_1 -> run_RefreshSim();

	// Show experimental result
	ddr3_refresh_1 -> showEval();
	if(ddr3_refresh_1 -> verify_DataIntegrity() == true)
		std::cout << "The data integrity of all rows are kept." << std::endl;
	else // certain row(s) was/were refreshed later than its/their retention time
		std::cout << "The data integrity is not kept." << std::endl;
}

void sim_approach_2(void)
{
	std::cout << "Running simulation: decay-based refresh counter" << std::endl << std::endl;
	ddr3_refresh_2 -> run_RefreshSim();

	// Show experimental result
	ddr3_refresh_2 -> showEval();
	if(ddr3_refresh_2 -> verify_DataIntegrity() == true)
		std::cout << "The data integrity of all rows are kept." << std::endl;
	else // certain row(s) was/were refreshed later than its/their retention time
		std::cout << "The data integrity is not kept." << std::endl;
}
