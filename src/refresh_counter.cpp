#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "../inc/refresh_counter.h"

using std::cout;
using std::cin;
using std::endl;

_SysTick_unit	round_length = (_SysTick_unit) tREFW;

RefreshCounter::RefreshCounter(_SysTick_unit &time_val, char *read_filename)
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

	// Initially, there is no data stored inside the DRAM
	// hence the invalid access duration of every sub-window is zero ns
	for(int i = 0; i < (int) PARTITION_NUM; i++)
		access_invalid[i] = (_SysTick_unit) 0;
}

// Initialising the value of all rows of any bank, with random value
void RefreshCounter::bank_init(int bank_id)
{
	for(unsigned int i = 0; i < (unsigned int) ROW_NUM; i++) 
		bank[bank_id].row[i] = i;
	
	for(unsigned int i = 0; i < (unsigned int) ROW_GP_NUM; i++)
		bank[bank_id].access[i] = 0x00;

	bank[bank_id].refresh_flag = (unsigned int) 0;
}

// Showing all value inside one bank
void RefreshCounter::view_bank(int bank_id)
{
	cout << "=================================" << endl;
	for(unsigned int i = 0; i < (unsigned int) ROW_NUM; i++)
		cout << "Bank[" << bank_id << "].row[" << i
			  << "]: " << bank[bank_id].row[i] << endl;
	cout << "=================================" << endl;
}

void RefreshCounter::update_row_group(int bank_id, int group_id, UpdateOp operation)
{
	if(operation = (UpdateOp) INC)
		bank[bank_id].access[group_id] += 0x01;
	else // (UpdateOp) DEC
		bank[bank_id].access[group_id] -= 0x01;
}

void RefreshCounter::refresh_row_group(int bank_id, int group_id)
{
	bank[bank_id].access[group_id] += 0x02;	
}

void RefreshCounter::pop_pattern(void)
{
	request_type.pop_back(); 
	request_size.pop_back();
	target_rg.pop_back();
	request_time.pop_back();
}

void RefreshCounter::run_RefreshSim(void)
{
	printf("RefreshTime: 0 ns\r\n"); cout << endl << endl;
	while(HyperPeriod_cnt <= (int) HYPER_PERIOD) {
		_SysTick_unit temp = time_update();
		accessed_checkpoint((temp / time_interval) - 1);
		printf("RefreshTime: %u ns\r\n", temp); 
		if((int) temp == (int) round_length) {
			HyperPeriod_cnt += 1;
			cout << "================== Refresh Window_" << HyperPeriod_cnt << " ================" << endl;
		}
	}
}

/**
  * @brief Checking if there was any newl arrival memory access within the sub-window of certain partition
  * @param Partition ID to decide the sub-window
**/   	
bool RefreshCounter::accessed_checkpoint(unsigned int par_id)
{
	// Identifying the valid access duration within the partition sub-window
	_SysTick_unit access_valid_max = (HyperPeriod_cnt - 1) * round_length + round_time - access_invalid[par_id] - 1;
	_SysTick_unit access_valid_min = (HyperPeriod_cnt - 1) * round_length + round_time - time_interval;

	unsigned int cur_level = RG_FIFO[par_id].cur_length;
	while(request_time.back() >= access_valid_min && request_time.back() <= access_valid_max) {
		RG_FIFO[par_id].row_group[cur_level] = target_rg.back();
		RG_FIFO[par_id].access_size[cur_level] = request_size.back(); 
		RG_FIFO[par_id].access_type[cur_level].assign(request_type.back());
		
		cout << "\t" << request_time.back() << "ns -> " << request_type.back().c_str() << " request ("
		     << request_size.back() << "-Byte" << endl;
		cur_level += 1; 
		pop_pattern();
	}
	RG_FIFO[par_id].cur_length = cur_level;
	access_invalid[par_id] = RG_FIFO[par_id].cur_length * (unsigned int) tRFC;
}

/**
  * @brief Initialising the time interval of refresh operation
  * @param refresh-interval parameter
**/   	
void RefreshCounter::refresh_partition(unsigned int par_id)
{

}

/**
  * @brief Initialising the time interval of refresh operation
  * @param refresh-interval parameter
**/   	
RetentionTimer::RetentionTimer(_SysTick_unit &time_val)
{
	time_unit_config(time_val);
	round_time = (_SysTick_unit) 0;
}

/**
  * @brief Configure the refresh interval
  * @param refresh-interval parameter
**/   	
_SysTick_unit RetentionTimer::time_unit_config(_SysTick_unit &time_val)
{
	time_interval = time_val;
	return time_interval;
}

/**
  * @brief Incrementing the timer counter once every given refresh interval
**/   	
_SysTick_unit RetentionTimer::time_update(void)
{
	round_time += time_interval;
	round_time = (round_time == (int) round_length + time_interval) ? time_interval : round_time;
	return round_time;
} 
