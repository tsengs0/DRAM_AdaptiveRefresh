#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include "../inc/refresh_counter.h"

using std::cout;
using std::cin;
using std::endl;

int solution_x;

_SysTick_unit	round_length = (_SysTick_unit) tREFW;

template<class InputIterator, class T>
bool RefreshCounter::search_FIFO(InputIterator first, InputIterator last, const T& val)
{
  // To record the iteration for marking the index for target querying row group inside the partition FIFO, if it is found somewhere
  unsigned int temp; temp = (unsigned int) 0;
  while (first!=last) {
    if (*first==val) {
	query_row_group = temp;
	return true;
   }
    temp += 1;
    ++first;
  }
  return false;
}

bool RefreshCounter::search_multiFIFO(unsigned int par_id, unsigned int cur_level)
{
	bool temp;
	for(unsigned int i = 0; i < (unsigned int) PARTITION_NUM; i++) {
	  if(i == par_id)
	    temp = search_FIFO(RG_FIFO[i].row_group, RG_FIFO[i].row_group + cur_level, target_rg.back());
	  else
	    temp = search_FIFO(RG_FIFO[i].row_group, RG_FIFO[i].row_group + RG_FIFO[i].cur_length, target_rg.back());

	  if(temp == true) {
		query_partition = i;
		return temp;
	  }
	}
	return temp;
}

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

	// Initialisation of evaluation parameters
	for(int i = 0; i < (int) SOLUTION_NUM; i++) {
		refresh_latency[i] = (unsigned long long) 0;
		valid_bus_time[i] = (_SysTick_unit) 0;
	}
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
	printf("RefreshTime: 0 ns\r\n");
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
void  RefreshCounter::accessed_checkpoint(unsigned int par_id)
{
	// Identifying the valid access duration within the partition sub-window
	_SysTick_unit sub_window_max   = (HyperPeriod_cnt - 1) * round_length + round_time - 1;
	_SysTick_unit access_valid_max = (HyperPeriod_cnt - 1) * round_length + round_time - access_invalid[par_id] - 1;
	_SysTick_unit access_valid_min = (HyperPeriod_cnt - 1) * round_length + round_time - time_interval;
	unsigned int invalid_request_cnt; invalid_request_cnt = (unsigned int) 0;
	unsigned int cur_level = RG_FIFO[par_id].cur_length;

	// Accumulating the invalid bus utilising time
	acc_validBusTime(access_valid_min, access_valid_max);	

	// Just for print out the valid and invalid durations for debugging
	if(
		(request_time.size() != 0) && 
		(request_time.back() >= access_valid_min && request_time.back() <= access_valid_max)
	) {
	  cout << "Access_valid(min,max) = " << "(" << access_valid_min - (HyperPeriod_cnt - 1)*round_length << ", " 
	       << access_valid_max - (HyperPeriod_cnt - 1)*round_length << "); "
	       << "Access_invalid: " << access_invalid[par_id] << endl;

	}
	while(
		(request_time.size() != 0) && 
		(request_time.back() >= access_valid_min && request_time.back() <= sub_window_max )
	) {
		// Enqueuing the arrival requests into partition FIFO if it arrived at valid duration
		if(request_time.back() <= access_valid_max) {
			// If the arrival request has been recorded inside any partition FIFO, just skip the following step
			if(search_multiFIFO(par_id, cur_level) == false) {
				RG_FIFO[par_id].row_group[cur_level] = target_rg.back();
				RG_FIFO[par_id].access_size[cur_level] = request_size.back(); 
				RG_FIFO[par_id].access_type[cur_level].assign(request_type.back());
				
				// Reset the retention time of newly arrival request's corresponding row group
				reset_retention(par_id, cur_level);
				cur_level += 1; 
			}
			else { // If the newly arrival request has been equeued in any partition FIFO
			       // just refreshing its corresponding row group by resetting its retentin time to64 ms
				reset_retention(query_partition, query_row_group);
			}
			
			cout << "\t" << request_time.back() << "ns -> " << request_type.back().c_str() << " request ("
			     << request_size.back() << "-Byte) targetting to row group (" << target_rg.back() << ")" << endl;
			pop_pattern(); 
		}
		else { // Procrastinating the arrival requests arriving at invalid duration
		       // Assuming those requested will be accesses at beginning of next sub-window. 
		       // Note that, the real accessed timing need to depend on the command scheduler, 
		       // we will leave as future work after merging this refresh mechansim into existing DRAM simulator
		       request_time[request_time.size() - 1 - invalid_request_cnt] = sub_window_max + 1 + invalid_request_cnt;
		       cout << "\t\t#Procrastinating Request's ideal access time to " << request_time[request_time.size() - 1 - invalid_request_cnt] << " ns (target row group: " 
			    << target_rg[target_rg.size() - 1 - invalid_request_cnt] << ")" << endl;
		       invalid_request_cnt -= 1;
		}
	}

	// 1) Decaying all row groups apart from the current checkpoint's corresponding partition FIFO
	// 2) Refreshing row groups of current checkpoint's corresponding partition FIFO
	for(unsigned int i = 0; i < (unsigned int) PARTITION_NUM; i++) {
		if(i == par_id) refresh_partition(i); 
		else            decay_partition(i);
	}
	RG_FIFO[par_id].cur_length = cur_level;
	access_invalid[par_id] = RG_FIFO[par_id].cur_length * (unsigned int) tRFC;
}

void RefreshCounter::reset_retention(unsigned int par_id, unsigned int cur_rg)
{
	RG_FIFO[par_id].RowGroup_retention[cur_rg] = (_SysTick_unit) 0; // (_SysTick_unit) tRetention;
}

void RefreshCounter::decay_retention(unsigned int par_id, unsigned int cur_rg, _SysTick_unit decay_time)
{
	RG_FIFO[par_id].RowGroup_retention[cur_rg] += decay_time;
}

void RefreshCounter::decay_partition(unsigned int par_id)
{
	unsigned int temp = RG_FIFO[par_id].cur_length;
	_SysTick_unit decay_time = (_SysTick_unit) tRetention / (_SysTick_unit) PARTITION_NUM;
	for(unsigned int i = 0; i < temp; i++)
		decay_retention(par_id, i, decay_time);
}
void RefreshCounter::acc_validBusTime(_SysTick_unit valid_min, _SysTick_unit valid_max)
{
	if(solution_x == (int) SOLUTION_1) {
		valid_bus_time[(int) SOLUTION_1] += (valid_max - valid_min);
	}
	else if(solution_x == (int) SOLUTION_2) {
		valid_bus_time[(int) SOLUTION_2] += (valid_max - valid_min);
	}
}

double RefreshCounter::calc_netBandwidth(void)
{
	_SysTick_unit elapsed_time = (HyperPeriod_cnt - 1) * round_length + round_time;

	if(solution_x == (int) SOLUTION_1) {
		return (double) (valid_bus_time[(int) SOLUTION_1] / elapsed_time);
	}
	else if(solution_x == (int) SOLUTION_2) {
		return (double) valid_bus_time[(int) SOLUTION_2] / (double) elapsed_time;
	}
}

void RefreshCounter::showEval(int trial_num)
{
	_SysTick_unit elapsed_time = (HyperPeriod_cnt - 1) * round_length + round_time;
	
	printf("The refresh-induced access latency under approach_%d: %llu ns\r\n", trial_num, refresh_latency[trial_num]);
	printf("The net bandwidth under approach_%d: %llu (ns) / %llu (ns) = %lf\%\r\n", trial_num, valid_bus_time[trial_num], elapsed_time, calc_netBandwidth() * 100);
}

/**
  * @brief Initialising the time interval of refresh operation
  * @param refresh-interval parameter
**/   	
void RefreshCounter::refresh_partition(unsigned int par_id)
{
	refresh_latency[1] += access_invalid[par_id];

	// After refreshing, the retention time of all row groups in the same partition become 64 ms 
	unsigned int Ref_cnt = RG_FIFO[par_id].cur_length;
	for(unsigned int i = 0; i < Ref_cnt; i++) 
		reset_retention(par_id, i);
}

bool RefreshCounter::verify_DataIntegrity(void)
{
	for(unsigned int i = 0; i < (unsigned int) PARTITION_NUM; i++) {
	  unsigned int cur_level = RG_FIFO[i].cur_length;
	  for(unsigned int j = 0; j < cur_level; j++) {
		_SysTick_unit temp = RG_FIFO[i].RowGroup_retention[j];
		printf("RG[%d].RowGroup[%d]'s data-holding time since last access or refresh point is: %f ms\r\n", i, j, (float) temp/1000000);
		if(temp > (_SysTick_unit) tRetention) 
		  return false;
	  }
	}
	return true;
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

