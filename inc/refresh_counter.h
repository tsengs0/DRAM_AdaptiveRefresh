#ifndef __REFRESH_COUNTER_H
#define __REFRESH_COUNTER_H

#include<iostream> 
#include<fstream> 
#include<string>
#include<sstream>
#include<vector>

#define REFRESH_COUNT 8192
#define RG_PER_BANK REFRESH_COUNT
#define ROW_GP_NUM (ROW_NUM / REFRESH_COUNT) // the number of rows in one refresh cycle
#define PARTITION_NUM 8
#define PARTITION_RG_NUM ((RG_PER_BANK * BANK_NUM) / PARTITION_NUM)

// DRAM device configuration for approach 1
#define SUB_WINDOW_NUM 4

#define DDR3_1600x2Gb
// Refer to Micron DDR3L-1333 (operating frequency: 1333 MHz)
#ifdef DDR3_1600x2Gb
	#define BANK_NUM 8
	#define ROW_NUM 16384
	#define COLUMN_NUM 1024
	#define DATA_WIDTH 16
	#define tRetention 64000000 // 64000000 ns = 64000 us = 64 ms
	#define tREFW tRetention 
	#define tREFI 7812.5 // 7812.5 ns = 7.812 us
	#define tRFC 128 // 260 ns
#elif DDR3_1600x2Gb
	#define ROW_NUM 1
#else
	
#endif

// Configuration for simulation
#define HYPER_PERIOD 3
#define SOLUTION_NUM 2
typedef unsigned int _SysTick_unit;
typedef unsigned int _Row_t;
typedef unsigned char _AccessCounter; // only last 3-bit are valid
typedef char UpdateOp;

enum {
	INC = 0x00,
	DEC = 0x01,
	REF = 0x02
};

typedef struct Row_Group {
	_Row_t row[ROW_GP_NUM];
	_AccessCounter access_cnt[];
} _RowGroup;

typedef struct Bank_t {
	// For ease of simulation, assume there is only one 1-bit cell inside each row
	_RowGroup row_group[RG_PER_BANK];
	unsigned int refresh_flag; // to track how many rows have already been refreshed

} _Bank;

typedef struct AccessTable_t {
	unsigned int access_row[RG_PER_BANK];
	unsigned int cur_length;
	unsigned int access_size[RG_PER_BANK];
	_AccessCounter access_cnt[RG_PER_BANK];
	std::string access_type[RG_PER_BANK];
	_SysTick_unit RowGroup_retention[RG_PER_BANK]; // for verification
} _AccessTable;

typedef struct Partition_fifo_t {
	// FIFO for each partition
	unsigned int row_group[PARTITION_RG_NUM];

	// For recording the number of row groups containing inside the FIFO
	unsigned int cur_length;

	// Each request size and request type
	unsigned int access_size[PARTITION_RG_NUM]; 
	std::string access_type[PARTITION_RG_NUM];

	// Additional parameter(s) for debugging and evaluation
	_SysTick_unit RowGroup_retention[PARTITION_RG_NUM]; 
} partition_fifo;

class RetentionTimer {
	private:

	protected:
		_SysTick_unit time_interval; // the time interval of one partition sub-window
		_SysTick_unit round_time;
	public:
		//~RetentionTimer();
		RetentionTimer(_SysTick_unit &time_val);

		_SysTick_unit time_unit_config(_SysTick_unit &time_val);
		_SysTick_unit time_update(void); 
		
};

// Approach 1: Access-aware refresh coutner
class AccessRefreshCounter : private RetentionTimer {
	private:
		// For access pattern
		std::vector<std::string> request_type; 
		std::vector<unsigned int> request_size, target_rg;
		std::vector<_SysTick_unit> request_time;
	
		_Bank bank[BANK_NUM];	
		_AccessTable access_track;
		unsigned int query_row_group;
		unsigned int HyperPeriod_cnt;

		// The parameters for evaluation
		_SysTick_unit valid_bus_time;
		_SysTick_unit lastRFC_time;
		unsigned long long int refresh_latency;	
	
	public:
		AccessRefreshCounter(_SysTick_unit &time_val, char *read_filename);
		//~RefreshCounter(void);
		void bank_init(int bank_id);
		void view_bank(int bank_id);
		
		void refresh_row_group(unsigned int group_id);
		void config_access_pattern(char *read_filename);
		void pop_pattern(void);

		// First proposed approach
		void update_row_group(unsigned int group_id, UpdateOp operation);
		void accessed_checkpoint(unsigned int sub_id);
		template<class InputIterator, class T> bool search_RGCounter(InputIterator first, InputIterator last, const T& val);
		void run_RefreshSim(void);
		void reset_retention(unsigned int group_id);
		void decay_retention(unsigned int group_id, _SysTick_unit decay_time);

		// Common functionalities for every approach
 		void acc_validBusTime(_SysTick_unit valid_min, _SysTick_unit valid_max);
	
		// Functions for evaluation
		double calc_netBandwidth(void);
		bool verify_DataIntegrity(void);
		void showEval(void);
};

// Approach 2: Decay-based refresh counter
class RefreshCounter : private RetentionTimer {
	private:
		// For access pattern
		std::vector<std::string> request_type; 
		std::vector<unsigned int> request_size, target_rg;
		std::vector<_SysTick_unit> request_time;
		std::vector<_MemMap> mem_map;
		
		_Bank bank[BANK_NUM];	
		unsigned int HyperPeriod_cnt;

		// Components for second approach
		partition_fifo RG_FIFO[PARTITION_NUM];
		_SysTick_unit access_invalid[PARTITION_NUM]; // determining the invalid access duratin within each sub-window, subject to tRFC
		unsigned int query_partition;
		unsigned int query_row_group;

		// The parameters for evaluation
		_SysTick_unit valid_bus_time;
		unsigned long long int refresh_latency;	
	
	public:
		RefreshCounter(_SysTick_unit &time_val, char *read_filename);
		//~RefreshCounter(void);
		void bank_init(int bank_id);
		void view_bank(int bank_id);
		
		void refresh_row_group(int bank_id, int group_id);
		void config_access_pattern(char *read_filename);
		void pop_pattern(void);

		// Second proposed approach
		void accessed_checkpoint(unsigned int par_id);
		void refresh_partition(unsigned int par_id);
		template<class InputIterator, class T> bool search_FIFO(InputIterator first, InputIterator last, const T& val);
		bool search_multiFIFO(unsigned int par_id, unsigned int cur_level);
		void run_RefreshSim(void);
		void reset_retention(unsigned int par_id, unsigned int cur_rg);
		void decay_retention(unsigned int par_id, unsigned int cur_rg, _SysTick_unit decay_time);
		void decay_partition(unsigned int par_id);

		// Common functionalities for every approach
 		void acc_validBusTime(_SysTick_unit valid_min, _SysTick_unit valid_max);
	
		// Functions for evaluation
		double calc_netBandwidth(void);
		bool verify_DataIntegrity(void);
		void showEval(void);
};

#endif // __REFRESH_COUNTER_H
