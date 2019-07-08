#ifndef __REFRESH_COUNTER_H
#define __REFRESH_COUNTER_H

#define BANK_NUM 8
#define ROW_NUM 32768
#define REFRESH_COUNT 8192
#define RG_PER_BANK REFRESH_COUNT
#define ROW_GP_NUM (ROW_NUM / REFRESH_COUNT) // the number of rows in one refresh cycle
#define PARTITION_NUM 8
#define PARTITION_RG_NUM ((RG_PER_BANK * BANK_NUM) / PARTITION_NUM)

#define DDR3_1333x4Gb
// Refer to Micron DDR3L-1333 (operating frequency: 1333 MHz)
#ifdef DDR3_1333x4Gb
	#define tREFW 64000000 // 64000000 ns = 64000 us = 64 ms
	#define tREFI 7812.5 // 7812.5 ns = 7.812 us
	#define tRFC 260 // 260 ns
#elif DDR3_1333x8Gb
	#define ROW_NUM 1
#else
	
#endif


// Configuration for simulation
#define HYPER_PERIOD 5

typedef unsigned int Row_t;
typedef unsigned char RowGroup_t; // only last 3-bit are valid
typedef char UpdateOp;

enum {
	INC = 0,
	DEC = 1
};

struct Bank_t {
	// For ease of simulation, assume there is only one 1-bit cell inside each row
	Row_t row[ROW_NUM]; 
	unsigned int refresh_flag; // to track how many rows have already been refreshed

	RowGroup_t access[ROW_GP_NUM]; 
};

class RetentionTimer {
	private:
		double time_interval; // the time interval of the timer
		double round_time;
	public:
		//~RetentionTimer();
		RetentionTimer(double &time_val);

		double time_unit_config(double &time_val);
		double time_update(void); 
		
};

class RefreshCounter : public RetentionTimer {
	private:
		struct Bank_t bank[BANK_NUM];	
		int HyperPeriod_cnt;
		
	public:
		RefreshCounter(double &time_val);
		//~RefreshCounter(void);
		void bank_init(int bank_id);
		void view_bank(int bank_id);
		void update_row_group(int bank_id, int group_id, UpdateOp operation);
		void refresh_row_group(int bank_id, int group_id);

		void run_RefreshSim(void);
};


#endif // __REFRESH_COUNTER_H
