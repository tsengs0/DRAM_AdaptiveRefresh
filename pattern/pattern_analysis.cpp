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
extern uint8_t bi_bc_pattern[MEM_MAP_SEL_NUM][2];

enum {
	REQUEST_TYPE  = 0, // Column 1 in .CSV File => Access type (WR or RD)
	REQUEST_SIZE  = 1, // Column 2 in .CSV File => Request Size of transaction (in unit of BYTE)
	TARGET_RG     = 2, // Column 3 in .CSV File => Target row group(s)
	BI_BC         = 3, // Column 3 in .CSV File => Target BI_BC combination
	REQUEST_TIME  = 4  // Column 4 in .CSV File => The access timing
};

typedef unsigned int _SysTick_unit;
typedef struct page_record_t {
	unsigned int page_addr;
	unsigned int access_num;
} page_record;

vector<page_record> PageRecord;

void rqst_rate_estimate(void);
int search_page(unsigned int &page_index);

// For access pattern
vector<std::string> request_type; 
vector<unsigned int> request_size, target_rg;
vector<_MemMap> bi_bc;
vector<_SysTick_unit> request_time;
string str;
short state; 

int main(int argc, char **argv)
{
	if(argc != 2) {
		cout << "Please give the filename of test pattern (*.csv)" << endl;
		exit(1);
	}

	cout << "Reading " << argv[1] << endl;
	// Reading the file
	ifstream ifs(argv[1]);
 	if(!ifs){
		cout<<"Read File Error";
		exit(1);
	}

    //Reading .CSV file column by column
    getline(ifs, str); // Skipping first line
    while(getline(ifs,str)) {
        string token;
        istringstream stream(str);
	state = (short) REQUEST_TYPE;

        while(getline(stream,token,',')) {
            if(state == (short) REQUEST_TYPE) {
		request_type.push_back(token.c_str());
		//cout << token.c_str() << ",";
	    }
            else if(state == (short) REQUEST_SIZE) {
		request_size.push_back(atoll(token.c_str()));
		//cout << atoll(token.c_str()) << ",";
	    }
            else if(state == (short) TARGET_RG) {
		target_rg.push_back(atoll(token.c_str()));
		//cout << atoll(token.c_str()) << ",";
	    }
            else if(state == (short) BI_BC) {
		uint8_t temp[3]; // BI, BC, Start Bank
		temp[0] = (uint8_t) atoi(token.c_str()); getline(stream, token, ',');
		temp[1] = (uint8_t) atoi(token.c_str()); getline(stream, token, ',');
		temp[2] = (uint8_t) atoi(token.c_str()); 
		
		bi_bc.push_back({temp[0], temp[1], temp[2]});
	    }
            else { // if(state == (short) REQUEST_TIME) {
		request_time.push_back(atoll(token.c_str()));
		//cout << atoll(token.c_str()) << ",";
	    }
	    state += 1;
        }
        //cout << endl;
    }
	for(unsigned int i = 0; i < target_rg.size(); i++) {
		search_page(target_rg[i]);
	}
	cout << PageRecord.size() << " pages will be accessed" << endl;
	for(unsigned int i = 0; i < PageRecord.size(); i++) { 
		cout << "Page_0x" << PageRecord[i].page_addr << " will be requested " << PageRecord[i].access_num << "times" << endl; 
	}
	ifs.close();

	return 0;
}
void rqst_rate_estimate(void);
int search_page(unsigned int &page_index) 
{
	unsigned int i; i = 0;

	if(PageRecord.size() == 0) {
		PageRecord.push_back({page_index, 1});
		return 0;
	}
	while(page_index != PageRecord[i].page_addr) {
		if(i >= PageRecord.size()) {
			PageRecord.push_back({page_index, 1});
			return 0;
		}
		i += 1;
	}
	PageRecord[i].access_num += 1;
	return 1;
}

