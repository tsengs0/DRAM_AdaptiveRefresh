#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include "../inc/refresh_counter.h"

using std::cout;
using std::cin;
using std::endl;
using std::vector;

extern _SysTick_unit round_length;

template<class InputIterator, class T>
bool AccessRefreshCounter::search_RGCounter(InputIterator first, InputIterator last, const T& val)
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

void AccessRefreshCounter::pop_pattern(void)
{
	request_type.pop_back(); 
	request_size.pop_back();
	target_rg.pop_back();
	request_time.pop_back();
}

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
/*	for(unsigned int i = 0; i < (unsigned int) RG_PER_BANK; i++) {
	  bank[bank_id].row_group[i].access_cnt = 0x00;
	  for(unsigned int j = 0; j < (unsigned int) ROW_GP_NUM; j++)
		bank[bank_id].row_group[i].row[j] = j;
	}

	bank[bank_id].refresh_flag = (unsigned int) 0;
*/
}

// Showing all value inside one bank
void AccessRefreshCounter::view_bank(int bank_id)
{
}

void AccessRefreshCounter::run_RefreshSim(void)
{
#ifdef DEBUG 
	printf("RefreshTime: 0 ns\r\n");
#endif
	while(HyperPeriod_cnt <= (unsigned int) HYPER_PERIOD) {
		_SysTick_unit temp = time_update();
		accessed_checkpoint((temp / time_interval) - 1); 
#ifdef DEBUG 
		printf("RefreshTime: %u ns\r\n", temp); 
#endif
		if((unsigned int) temp == (unsigned int) round_length) {
			HyperPeriod_cnt += 1;
#ifdef DEBUG 
			cout << "================== Refresh Window_" << HyperPeriod_cnt << " ================" << endl;
			printf("RefreshTime: 0 ns\r\n");
#endif
		}
	}
}

/**
  * @brief Checking if there was any newl arrival memory access within the sub-window of certain partition
  * @param Partition ID to decide the sub-window
**/   	
void AccessRefreshCounter::accessed_checkpoint(unsigned int sub_id)
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
			if(search_RGCounter(access_track.access_row, access_track.access_row + cur_level, target_rg.back()) == true) {
				update_row_group(query_row_group, (UpdateOp) INC);

				// Marking the row group accessed at current sub-window
				accessed_rg.push_back(query_row_group);

			}
			else { // If the target row group is first time time to be accessed, then 
			       // enqueuing it into access table and updating its retention counter 
				access_track.access_row[cur_level] = target_rg.back();
				access_track.access_size[cur_level] = request_size.back();
				access_track.access_type[cur_level].assign(request_type.back());
				access_track.access_cnt[cur_level] = 0x00; // initialising the access counter until data is really allocated to its corresponding row group				

				// Reset the retention time of newly arrival request's corresponding row group
				update_row_group(cur_level, (UpdateOp) INC);
				
				// Marking the row group accessed at current sub-window
				accessed_rg.push_back(cur_level);

				cur_level += 1; 
			}
			
#ifdef DEBUG 
			cout << "\t\t" << request_time.back() << "ns -> " << request_type.back().c_str() << " request ("
			     << request_size.back() << "-Byte) targetting to row group (" << target_rg.back() << ")" << endl;
#endif
			pop_pattern(); 
		}
		else if(request_time.back() >= sub_window_min && request_time.back() < access_valid_min) { 
		       // Procrastinating the arrival requests arriving at invalid duration
		       // Assuming those requested will be accesses at beginning of next sub-window. 
		       // Note that, the real accessed timing need to depend on the command scheduler, 
		       // we will leave as future work after merging this refresh mechansim into existing DRAM simulator
		       request_time.back() = sub_window_min  + invalid_request_cnt;
#ifdef DEBUG 
		       cout << "\t\t\t#Procrastinating Request's ideal access time to " << request_time.back() << " ns (target row group: " 
			    << target_rg.back() << ")" << endl;
#endif
		       invalid_request_cnt += 1;

			if(search_RGCounter(access_track.access_row, access_track.access_row + cur_level, target_rg.back()) == true) {
				update_row_group(query_row_group, (UpdateOp) INC);
				reset_retention(query_row_group); // for verification

				// Marking the row group accessed at current sub-window
				accessed_rg.push_back(query_row_group);

			}
			else { // If the target row group is first time time to be accessed, then 
			       // enqueuing it into access table and updating its retention counter 
				access_track.access_row[cur_level] = target_rg.back();
				access_track.access_size[cur_level] = request_size.back();
				access_track.access_type[cur_level].assign(request_type.back());
				access_track.access_cnt[cur_level] = 0x00; // initialising the access counter until data is really allocated to its corresponding row group				

				// Reset the retention time of newly arrival request's corresponding row group
				update_row_group(cur_level, (UpdateOp) INC);
				
				// Marking the row group accessed at current sub-window
				accessed_rg.push_back(cur_level);

				cur_level += 1; 
			}
			pop_pattern(); 
		}
		else {
			cout << "Wrong calculation of round timer" << endl;
			exit(1);
		}
	}
	
	//printf("From round time %u ns to %u ns, there came row-group access(es):\r\n", round_time - (round_length / (_SysTick_unit) SUB_WINDOW_NUM), round_time);
	//for(unsigned int i = 0; i < accessed_rg.size(); i++) printf("\t\tRow Group: %u\r\n", access_track.access_row[accessed_rg[i]]);

	// 1) Decrementing all row groups whose was not accessed within current sub-window
	// 2) Refreshing row groups whose value of access counter is smaller than 1;
	lastRFC_time = round_time; 
	for(unsigned int i = 0; i < (unsigned int) cur_level; i++) {
		// 1)
		if(	accessed_rg.size() != 0 ) {/*&&
			search_RGCounter(&(access_track.access_row[accessed_rg[0]]), &(access_track.access_row[accessed_rg.back()]), access_track.access_row[i]) == false
		) {*/
			for(unsigned int j = 0; j < accessed_rg.size(); j++) {
				if(access_track.access_row[ accessed_rg[j] ] == access_track.access_row[i]) {
					update_row_group(i, (UpdateOp) DEC);
					break;
				}
			}
		
		}

		// 2)
		if(access_track.access_cnt[i] < 1) {
			refresh_row_group(i);
			lastRFC_time += (_SysTick_unit) tRFC;
		}
	}
	
	access_track.cur_length = cur_level;	
	vector<unsigned int>().swap(accessed_rg);
}

void AccessRefreshCounter::update_row_group(unsigned int group_id, UpdateOp operation)
{
	if(operation == (UpdateOp) INC) {
		access_track.access_cnt[group_id] += 0x01;
		reset_retention(group_id); // for verification
	}
	else if(operation == (UpdateOp) DEC) {
		access_track.access_cnt[group_id] -= 0x01;
		decay_retention(group_id, (_SysTick_unit) tRetention / (_SysTick_unit) SUB_WINDOW_NUM); // for verification
	}
	else if(operation == (UpdateOp) REF) {
		access_track.access_cnt[group_id] += 0x02;
		reset_retention(group_id); // for verification
	}
	else {
		cout << "Undefined update-operation is issued" << endl;
		exit(1);
	}
}

void AccessRefreshCounter::refresh_row_group(unsigned int group_id)
{
#ifdef DEBUG 
	cout << " Issuing refresh command toward row group " << group_id << endl;
#endif
	refresh_latency += (_SysTick_unit) tRFC;
	update_row_group(group_id, (UpdateOp) REF);
}

void AccessRefreshCounter::reset_retention(unsigned int group_id)
{
	access_track.RowGroup_retention[group_id] = (_SysTick_unit) 0; // (_SysTick_unit) tRetention;
}

void AccessRefreshCounter::decay_retention(unsigned int group_id, _SysTick_unit decay_time)
{
	access_track.RowGroup_retention[group_id] += decay_time;
}

void AccessRefreshCounter::acc_validBusTime(_SysTick_unit valid_min, _SysTick_unit valid_max)
{
	valid_bus_time += (valid_max - valid_min);
}

bool AccessRefreshCounter::verify_DataIntegrity(void)
{
	unsigned int cur_level = access_track.cur_length;
	for(unsigned int i = 0; i < cur_level; i++) {
	  _SysTick_unit temp = access_track.RowGroup_retention[i];
#ifdef DEBUG 
	  printf("RowGroup[%d]'s data-holding time since last access or refresh point is: %f ms\r\n", i, (float) temp/1000000);
#endif
	   if(temp > (_SysTick_unit) tRetention) 
	 return false;
	}
	return true;
} 

double AccessRefreshCounter::calc_netBandwidth(void)
{
	_SysTick_unit elapsed_time = (HyperPeriod_cnt - 1) * round_length + round_time;

	return ((double) elapsed_time - (double) refresh_latency) / (double) elapsed_time; 
	//return (double) valid_bus_time / (double) elapsed_time;
}

void AccessRefreshCounter::showEval(void)
{
	_SysTick_unit elapsed_time = (HyperPeriod_cnt - 1) * round_length + round_time;
	
	printf("The refresh-induced access latency: %llu ns\r\n", refresh_latency);
	printf("The net bandwidth: %llu (ns) / %llu (ns) = %lf\%\r\n", (elapsed_time - refresh_latency), elapsed_time, calc_netBandwidth() * 100);
}
