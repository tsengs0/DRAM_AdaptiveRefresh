#include <iostream>
#include <cstdio>
#include <cstdlib>
#include<fstream> 
#include<string>
#include<sstream>
#include<vector>
#include <algorithm>
#include <cmath>
#include "../inc/mem_map.h"

using namespace std;

// Format of Test Pattern from RTMemController
enum {
	REQ_TIME = 0,
	REQ_TYPE = 1,
	REQ_ADDR = 2,
	REQ_SIZE = 3
};

enum {
	REQUEST_TYPE  = 0, // Column 1 in .CSV File => Access type (WR or RD)
	REQUEST_SIZE  = 1, // Column 2 in .CSV File => Request Size of transaction (in unit of BYTE)
	TARGET_RG     = 2, // Column 3 in .CSV File => Target row group(s)
	BI_BC         = 3, // Column 3 in .CSV File => Target BI_BC combination
	REQUEST_TIME  = 4  // Column 4 in .CSV File => The access timing
};

typedef unsigned int _SysTick_unit;
#define REFRESH_COUNT 8192
#define RG_PER_BANK REFRESH_COUNT
#define ROW_GP_NUM (ROW_NUM / REFRESH_COUNT) // the number of rows in one refresh cycle
#define PARTITION_NUM 8
#define PARTITION_RG_NUM ((RG_PER_BANK * BANK_NUM) / PARTITION_NUM)

#define clkMhz 800
#define BANK_NUM 8
#define ROW_NUM 16384
#define COLUMN_NUM 1024
#define DATA_WIDTH 16

int main(int argc, char **argv)
{
	// For access pattern
	std::vector<std::string> request_type; 
	std::vector<unsigned int> request_size, target_rg;
	std::vector<_SysTick_unit> request_time;
	std::vector<_MemMap> bi_bc;
	string str;
	unsigned DecLogic; // logical address
	short state; 
  
	// Memory Mapping from bank addressing
    	unsigned colBits = static_cast<unsigned> (log2( (unsigned) COLUMN_NUM));
	
	// Memory Mapping from row address to row group id
  	unsigned rowShift = static_cast<unsigned> (log2( (unsigned) COLUMN_NUM * (unsigned) BANK_NUM));
  	unsigned rowMask = static_cast<unsigned> (ROW_NUM - 1) << rowShift; // 4096
 
	cout << "Reading " << argv[1] << endl;
	// Reading the file
	ifstream ifs(argv[1]);
 	if(!ifs){
		cout<<"Read File Error";
		exit(1);
	}

    //Reading .CSV file column by column
   // getline(ifs, str); // Skipping first line
    while(getline(ifs,str)) {
        string token;
        istringstream stream(str);
	state = (short) REQUEST_TYPE;

        while(getline(stream,token,',')) {
            if(state == (short) REQ_TIME) {
		request_time.push_back((atoll(token.c_str()) / 800)*1000);
		//cout << atoll(token.c_str()) << ",";
	    }
            else if(state == (short) REQ_TYPE) {
		if(token == "read") request_type.push_back("RD");
		else if (token == "write") request_type.push_back("WD");
		else request_type.push_back("NOP");
		//cout << token.c_str() << ",";
	    }
            else if(state == (short) REQ_ADDR) {
		target_rg.push_back(stoul(token, nullptr, 0));
		DecLogic = target_rg.back();
		target_rg.back() = (target_rg.back() & rowMask) >> rowShift;
		target_rg.back() = target_rg.back() / (unsigned int) ROW_GP_NUM;
		//cout << atoll(token.c_str()) << ",";
	    }
            else { // if(state == (short) REQ_SIZE) {
		request_size.push_back(atoll(token.c_str()));
		//cout << atoll(token.c_str()) << ",";
	        
		int pattern_index;
		pattern_index = (request_size.back() == 16) ? REQUEST_16B :
				(request_size.back() == 32) ? REQUEST_32B :
				(request_size.back() == 64) ? REQUEST_64B :
				(request_size.back() == 128) ? REQUEST_128B :
				(request_size.back() == 256) ? REQUEST_256B : REQUEST_16B;

		unsigned BI = bi_bc_pattern[pattern_index][BI_INDEX];
		unsigned BC = bi_bc_pattern[pattern_index][BC_INDEX];
    		unsigned bankShift = static_cast<unsigned> (colBits + ((BI > 1) ? log2(BI) : 0));
    		unsigned bankMask = static_cast<unsigned> ((unsigned) BANK_NUM / (unsigned) BI - 1) << bankShift;
    		unsigned bankAddr = (DecLogic & bankMask) >> colBits;
		bi_bc.push_back({(uint8_t) BI, (uint8_t) BC, (uint8_t) bankAddr});
	    }
	    state += 1;
        }
       // cout << endl;
    }

/*	for(unsigned int i = 0; i < request_type.size(); i++) {
		printf("Request_%d, %s, %dB, RG_%d, %u ns\r\n", i, request_type[i].c_str(), request_size[i], target_rg[i], request_time[i]);
	}

*/	
	ofstream ofs(argv[2]); 
	ofs << "# Transaction type, request size (Byte), target row group (bank interleaving), BI, BC, Start Bank, access time (ns)" << endl;
	for(unsigned int i = 0; i < request_type.size(); i++) {
		ofs << request_type[i].c_str() << ","  
		    << request_size[i] << ","
	      	    << target_rg[i] << ","
	      	    << (unsigned int) bi_bc[i].BI << ","
	      	    << (unsigned int) bi_bc[i].BC << ","
	      	    << (unsigned int) bi_bc[i].start_bank << ","
	    	    << request_time[i] << endl;
	}
	ifs.close();
	//system("mv *.csv CombinedMemTrace/");
}
