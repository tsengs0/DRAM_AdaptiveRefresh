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
	HyperPeriod_cnt = (unsigned int) 1;

	// Configuring the access patterns
	config_access_pattern(read_filename);

	// 1) Initially, there is no data stored inside the DRAM
	//    hence the invalid access duration of every sub-window is zero ns
	for(unsigned int i = 0; i < (unsigned int) SUB_WINDOW_NUM; i++)
		access_invalid[i] = (_SysTick_unit) 0;

	// Initialisation of Tracking Registers
	access_track.cur_length = (unsigned int) 0;

	// Initialisation of evaluation parameters
	lastRFC_time = (_SysTick_unit) 0;
	refresh_latency = (unsigned long long) 0;
	valid_bus_time = (_SysTick_unit) 0;
}

// Initialising the value of all rows of any bank, with random value
void AccessRefreshCounter::bank_init(int bank_id)
{
	for(unsigned int i = 0; i < (unsigned int) RG_PER_BANK_COUNT; i++) {
	  bank[bank_id].row_group[i].access_cnt = 0x00;
	  for(unsigned int j = 0; j < (unsigned int) ROW_GP_NUM; j++)
		bank[bank_id].row_group[i].row[j] = j;
	}

	bank[bank_id].refresh_flag = (unsigned int) 0;
}

// Showing all value inside one bank
void RefreshCounter::view_bank(int bank_id)
{
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

/**
  * @brief Checking if there was any newl arrival memory access within the sub-window of certain partition
  * @param Partition ID to decide the sub-window
**/   	
void Access RefreshCounter::accessed_checkpoint(unsigned int sub_id)
{

	// Identifying the valid access duration within the sub-window
	_SysTick_unit sub_window_min   = (HyperPeriod_cnt - 1) * round_length;
	_SysTick_unit access_valid_max = (HyperPeriod_cnt - 1) * round_length + round_time - 1;
	_SysTick_unit access_valid_min = (HyperPeriod_cnt - 1) * round_length + lastRFC_time;
	unsigned int cur_level = access_track.cur_length;
	vector<unsigned int> accessed_rg; // tempoarily marking the row group(s) accessed within the current sub-window
	unsigned int invalid_request_cnt; invalid_request_cnt = (unsigned int) 0;

	// Accumulating the invalid bus utilising time
	acc_validBusTime(access_valid_min, access_valid_max);	

	while(
		(request_time.size() != 0) && 
		(request_time.back() <= access_valid_max )
	) {
		if(request_time.back() >= access_valid_min) {
			// If the target row group of arrival request had been accessed (recorded in access table), 
			// then just updating its retention counter 
			if(search_multiFIFO(sub_id, cur_level) == true) {
				update_row_group(query_row_group, (UpdateOp) INC);
				reset_retention(query_row_group); // for verification

				// Marking the row group accessed at current sub-window
				accessed_rg.push_back(query_row_group);

			}
			else { // If the target row group is first time time to be accessed, then 
			       // enqueuing it into access table and updating its retention counter 
				access_track.access_row[cur_level] = target_rg.bach();
				access_track.access_size[cur_level] = request_size.back();
				access_track.access_type[cur_level].assign(request_type.back());

				// Reset the retention time of newly arrival request's corresponding row group
				update_row_group(target_rg.back(), (UpdateOp) INC);
				reset_retention(target_rg.back()); // for verification
				
				// Marking the row group accessed at current sub-window
				accessed_rg.push_back(query_row_group);

				cur_level += 1; 
			}
			
			cout << "\t\t" << request_time.back() << "ns -> " << request_type.back().c_str() << " request ("
			     << request_size.back() << "-Byte) targetting to row group (" << target_rg.back() << ")" << endl;
			pop_pattern(); 
		}
		else if(request_time.back() >= sub_window_min && request_time.back() < access_valid_min) { 
		       // Procrastinating the arrival requests arriving at invalid duration
		       // Assuming those requested will be accesses at beginning of next sub-window. 
		       // Note that, the real accessed timing need to depend on the command scheduler, 
		       // we will leave as future work after merging this refresh mechansim into existing DRAM simulator
		       request_time.back() = sub_window_min  + invalid_request_cnt;
		       cout << "\t\t\t#Procrastinating Request's ideal access time to " << request_time.back() << " ns (target row group: " 
			    << target_rg.back() << ")" << endl;
		       invalid_request_cnt += 1;

			if(search_multiFIFO(sub_id, cur_level) == true) {
				update_row_group(query_row_group, (UpdateOp) INC);
				reset_retention(query_row_group); // for verification

				// Marking the row group accessed at current sub-window
				accessed_rg.push_back(query_row_group);

			}
			else { // If the target row group is first time time to be accessed, then 
			       // enqueuing it into access table and updating its retention counter 
				access_track.access_row[cur_level] = target_rg.bach();
				access_track.access_size[cur_level] = request_size.back();
				access_track.access_type[cur_level].assign(request_type.back());

				// Reset the retention time of newly arrival request's corresponding row group
				update_row_group(target_rg.back(), (UpdateOp) INC);
				reset_retention(target_rg.back()); // for verification
				
				// Marking the row group accessed at current sub-window
				accessed_rg.push_back(query_row_group);

				cur_level += 1; 
			}
			pop_pattern(); 
		}
		else {
			cout << "Wrong calculation of round timer" << endl;
			exit(1);
		}
	}

	// 1) Decrementing all row groups whose was not accessed within current sub-window
	for(unsigned int i = 0; i < (unsigned int) cur_level; i++) {
		if(search_FIFO(accessed_rg.begin(), accessed_rg.end(), access_track.access_row[i]) == false)
			update_row_group(i, (UpdateOp) DEC);
			reset_retention(i); // for verification
	}
	
	// 2) Refreshing row groups whose value of access counter is smaller than 1;
	for(unsigned int i = 0; i < (unsigned int) cur_level; i++) {
		if(access_track)
	}

	RG_FIFO[par_id].cur_length = cur_level; 
	access_invalid[par_id] = RG_FIFO[par_id].cur_length * (unsigned int) tRFC;
}
