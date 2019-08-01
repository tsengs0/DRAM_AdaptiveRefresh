#include <algorithm>
#include "../inc/refresh_counter.h"
using namespace std;

enum {
	REQUEST_TYPE  = 0, // Column 1 in .CSV File => Access type (WR or RD)
	REQUEST_SIZE  = 1, // Column 2 in .CSV File => Request Size of transaction (in unit of BYTE)
	TARGET_RG     = 2, // Column 3 in .CSV File => Target row group(s)
	BI_BC         = 3, // Column 3 in .CSV File => Target BI_BC combination
	REQUEST_TIME  = 4  // Column 4 in .CSV File => The access timing
};

void AccessRefreshCounter::config_access_pattern(char *read_filename)
{
	string str;
	short state; 
 
	cout << "Reading " << read_filename << endl;
	// Reading the file
	ifstream ifs(read_filename);
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

	reverse(request_type.begin(), request_type.end());   // note: reverse iterators
	reverse(request_size.begin(), request_size.end());   // note: reverse iterators
	reverse(target_rg.begin(), target_rg.end());   // note: reverse iterators
	reverse(bi_bc.begin(), bi_bc.end());   // note: reverse iterators
	reverse(request_time.begin(), request_time.end());   // note: reverse iterators
#ifdef DEBUG
	for(unsigned int i = 0; i < request_type.size(); i++) {
		printf("Request_%d, %s, %dB, RG_%d, %d, %d, %d, %u ns\r\n", i, request_type[i].c_str(), request_size[i], target_rg[i], (int) bi_bc[i].BI, (int) bi_bc[i].BC, (int) bi_bc[i].start_bank, request_time[i]);
	}
#endif
	ifs.close();
}

void RefreshCounter::config_access_pattern(char *read_filename)
{
	string str;
	short state; 
 
	cout << "Reading " << read_filename << endl;
	// Reading the file
	ifstream ifs(read_filename);
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

	reverse(request_type.begin(), request_type.end());   // note: reverse iterators
	reverse(request_size.begin(), request_size.end());   // note: reverse iterators
	reverse(target_rg.begin(), target_rg.end());   // note: reverse iterators
	reverse(bi_bc.begin(), bi_bc.end());   // note: reverse iterators
	reverse(request_time.begin(), request_time.end());   // note: reverse iterators
#ifdef DEBUG
	for(unsigned int i = 0; i < request_type.size(); i++) {
		printf("Request_%d, %s, %dB, RG_%d, %d, %d, %d, %u ns\r\n", i, request_type[i].c_str(), request_size[i], target_rg[i], (int) bi_bc[i].BI, (int) bi_bc[i].BC, (int) bi_bc[i].start_bank, request_time[i]);
	}
#endif
	ifs.close();
}
