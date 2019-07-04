#ifndef __REFRESH_COUNTER_H
#define __REFRESH_COUNTER_H

#define BANK_NUM 8
#define ROW_NUM 32768
#define REFRESH_COUNT 8192
#define ROW_GP_NUM (ROW_NUM / REFRESH_COUNT) // the number of rows in one refresh cycle

// Refer to Micron DDR3L-800 (operating frequency: 800 MHz)
#define tREFW 64000000 // 64000000 ns = 64000 us = 64 ms
#define tREFI 7812.5 // 7812.5 ns = 7.812 us
#define tRFC 260 // 260 ns

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

class RefreshCounter {
	private:
		struct Bank_t bank[BANK_NUM];	
		
		
	public:
		RefreshCounter(void);
		//~RefreshCounter(void);
		void bank_init(int bank_id);
		void view_bank(int bank_id);
		void update_row_group(int bank_id, int group_id);
		void refresh_row_group(int bank_id, int group_id);
};



#endif // __REFRESH_COUNTER_H
