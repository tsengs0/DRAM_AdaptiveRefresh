#include <iostream>
#include "../inc/refresh_counter.h"


RefreshCounter::RefreshCounter(void)
{
	for(int i = 0; i < (int) BANK_NUM; i++) {
		bank_init(i);
		//view_bank(i);
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
	std::cout << "=================================" << std::endl;
	for(unsigned int i = 0; i < (unsigned int) ROW_NUM; i++)
		std::cout << "Bank[" << bank_id << "].row[" << i
			  << "]: " << bank[bank_id].row[i] << std::endl;
	std::cout << "=================================" << std::endl;
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
