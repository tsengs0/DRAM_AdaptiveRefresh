#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "../inc/refresh_counter.h"

using std::cout;
using std::cin;
using std::endl;

RefreshCounter::RefreshCounter(double &time_val)
               : RetentionTimer(time_val) // Initialisation list
{
	for(int i = 0; i < (int) BANK_NUM; i++) {
		bank_init(i);
		//view_bank(i);
	}
	
	// Since the simulator is designed to be run for certain number of refresh windows (hper-period).
	// Thus, a counter to record the currently elapsed refresh window is necessary.
	HyperPeriod_cnt = (int) 0;
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

void RefreshCounter::run_RefreshSim(void)
{
	while(HyperPeriod_cnt < (int) Hyper_Period) {
		double temp = time_update();
		printf("RefreshTime: %05f ns\r\n", temp);
		HyperPeriod_cnt = (temp == (double) tREFW) ? HyperPeriod_cnt + 1 : HyperPeriod_cnt;
	}
}


/**
  * @brief Initialising the time interval of refresh operation
  * @param refresh-interval parameter
**/   	
RetentionTimer::RetentionTimer(double &time_val)
{
	time_unit_config(time_val);
	round_time = (double) 0.0;
}

/**
  * @brief Configure the refresh interval
  * @param refresh-interval parameter
**/   	
double RetentionTimer::time_unit_config(double &time_val)
{
	time_interval = time_val;
	return time_interval;
}

/**
  * @brief Incrementing the timer counter once every given refresh interval
**/   	
double RetentionTimer::time_update(void)
{
	round_time += time_interval;
	return round_time;
} 
