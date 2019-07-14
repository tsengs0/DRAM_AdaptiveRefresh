#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include "../inc/refresh_counter.h"

using std::cout;
using std::cin;
using std::endl;

extern _SysTick_unit round_length;

AccessRefreshCounter::AccessRefreshCounter(_SysTick_unit &time_val, char *read_filename)
               : RetentionTimer(time_val) // Initialisation list
{
	for(int i = 0; i < (int) BANK_NUM; i++) {
		bank_init(i);
		//view_bank(i);
	}
	
	// Since the simulator is designed to be run for certain number of refresh windows (hper-period).
	// Thus, a counter to record the currently elapsed refresh window is necessary.
	HyperPeriod_cnt = (int) 1;

	// Configuring the access patterns
	config_access_pattern(read_filename);

	// 1) Initially, there is no data stored inside the DRAM
	//    hence the invalid access duration of every sub-window is zero ns
	for(int i = 0; i < (int) SUB_WINDOW_NUM; i++)
		access_invalid[i] = (_SysTick_unit) 0;

	// Initialisation of evaluation parameters
	refresh_latency = (unsigned long long) 0;
	valid_bus_time = (_SysTick_unit) 0;
}

// Initialising the value of all rows of any bank, with random value
void AccessRefreshCounter::bank_init(int bank_id)
{
	for(unsigned int i = 0; i < (unsigned int) ROW_NUM; i++) 
		bank[bank_id].row[i] = i;
	
	for(unsigned int i = 0; i < (unsigned int) ROW_GP_NUM; i++)
		bank[bank_id].access[i] = 0x00;

	bank[bank_id].refresh_flag = (unsigned int) 0;
}

void AccessRefreshCounter::update_row_group(int bank_id, int group_id, UpdateOp operation)
{
	if(operation = (UpdateOp) INC)
		bank[bank_id].access[group_id] += 0x01;
	else // (UpdateOp) DEC
		bank[bank_id].access[group_id] -= 0x01;
}

void AccessRefreshCounter::refresh_row_group(int bank_id, int group_id)
{
	bank[bank_id].access[group_id] += 0x02;	
}

void AccessRefreshCounter::run_RefreshSim(void)
{
	printf("RefreshTime: 0 ns\r\n");
	while(HyperPeriod_cnt <= (int) HYPER_PERIOD) {
		_SysTick_unit temp = time_update();
	//	accessed_checkpoint((temp / time_interval) - 1); 
		printf("RefreshTime: %u ns\r\n", temp); 
		if((int) temp == (int) round_length) {
			HyperPeriod_cnt += 1;
			cout << "================== Refresh Window_" << HyperPeriod_cnt << " ================" << endl;
			printf("RefreshTime: 0 ns\r\n");
		}
	}
}
