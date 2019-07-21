#include "../inc/refresh_counter.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <algorithm>

using namespace std;

enum {
	REQUEST_TYPE  = 0, // Column 1 in .CSV File => Access type (WR or RD)
	REQUEST_SIZE  = 1, // Column 2 in .CSV File => Request Size of transaction (in unit of BYTE)
	TARGET_RG     = 2, // Column 3 in .CSV File => Target row group
	REQUEST_TIME  = 3  // Column 4 in .CSV File => The access timing
};

#define title_1 "# Transaction type"
#define title_2 "request size (Byte)"
#define title_3 "target row group (bank interleaving)"
#define title_4 "access time (ns)"

int request_num, ReqRate_min, ReqRate_max;
void param_config(char **argv);
_SysTick_unit *first_arrival;

  //translate the logical address to physical address.  
  PhysicalAddr physicalAddr = addressDecoder->MemoryMap(transaction);
  transaction.physicalAddr = physicalAddr;  
int main(int argc, char **argv)
{
	string str;
	short state; 
	char output_filename[50];

	srand((unsigned) time(NULL));

	if(argc != 5) {
		cout << "Wrong format of parameter passing." << endl
		     << "1) Filename of generated file, 2) number of requestor (integer only), 3) minimum request rate (%), 4) maximum request rate (%)" << endl;
	}
	 	
	// Configuring the parameters
	param_config(argv);
	
/*
	// Generating the file
	cout << "Generating the file of access pattern" << endl;
	sprintf(output_file,"%s.csv\0", argv[1]);
	ofstream ofs(output_fileName); 
	ofs << file_index << ","  
	    << rfj << ","
	    << afj << ","
	    << TargetResponse << ","
	    << AverageResponse << ","
	    << StandardDeviation << ","
	    << energy << ","
	    << miss_dline << endl;
	ofs.close();
*/	
	return 0;
}

void param_config(char **argv)
{
	request_num = atoi(argv[2]);
	if(!(request_num >= 1 && request_num <= (int) RG_PER_BANK)) {
		cout << "The number of requestors should be in [" << 0 << ", " << (int) RG_PER_BANK << "]" << endl; 
		exit(1);
	}

	ReqRate_min = atoi(argv[3]);
	if(!(ReqRate_min > 0 && ReqRate_min <= 100)) {
		cout << "The maximum request rate should be in [" << 0 << ", " << 100 << "]" << endl; 
		exit(1);
	}
	
	ReqRate_max = atoi(argv[4]);
	if(!(ReqRate_max > 0 && ReqRate_max <= 100)) {
		cout << "The request rate should be in [" << 0 << ", " << 100 << "]" << endl; 
		exit(1);
	}

	if(ReqRate_min > ReqRate_max) {
		cout << "The minimum request rate is larger than maximum request rate now" << endl;
		exit(1);
	}

	first_arrival = new _SysTick_unit[request_num];

	for(int i = 0; i < request_num; i++) {
		first_arrival[i] = (rand() % ((int)PARTITION_NUM * (int)HYPER_PERIOD) + 1) * ((_SysTick_unit) tREFW / (_SysTick_unit) PARTITION_NUM); 
		cout << "First_arrival[" << i << "]: " << first_arrival[i] << endl;
	}
}
