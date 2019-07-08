#include "../inc/refresh_counter.h"
using namespace std;

enum {
	REQUEST_TYPE  = 0, // Column 1 in .CSV File => Access type (WR or RD)
	REQUEST_SIZE  = 1, // Column 2 in .CSV File => Request Size of transaction (in unit of BYTE)
	TARGET_RG     = 2, // Column 3 in .CSV File => Target row group
	REQUEST_TIME  = 3  // Column 4 in .CSV File => The access timing
};


void RefreshCounter::config_access_pattern(char *read_filename)
{
	string str;
	short state; 
 
	char filename[100], output_FileName[100];


	cout << "Reading " << read_filename << endl;
	// Reading the file
	ifstream ifs(read_filename);
 	if(!ifs){
		cout<<"Read File Error";
		exit(1);
	}

    //Reading .CSV file column by column
    while(getline(ifs,str)) {
        string token;
        istringstream stream(str);
	state = (short) REQUEST_TYPE;

        while(getline(stream,token,',')) {
            if(state == (short) REQUEST_TYPE) {
		request_type.push_back(token.c_str());
		cout << token.c_str() << ",";
	    }
            else if(state == (short) REQUEST_SIZE) {
		request_size.push_back(atoi(token.c_str()));
		cout << atoi(token.c_str()) << ",";
	    }
            else if(state == (short) TARGET_RG) {
		target_rg.push_back(atoi(token.c_str()));
		cout << atoi(token.c_str()) << ",";
	    }
            else { // if(state == (short) REQUEST_TIME) {
		request_time.push_back(stof(token));
		cout << stof(token) << ",";
	    }
	    state += 1;
        }
        cout << endl;
    }

	for(int i = 0; i < request_type.size(); i++) {
		printf("Request_%d, %s, %dB, RG_%d, %02f ns\r\n", i, request_type[i].c_str(), request_size[i], target_rg[i], request_time[i]);
	}

	/*
	sprintf(output_FileName,"Total_%s.%s.csv", argv[1], argv[2]);
	ofstream ofs(output_FileName); 
	ofs << file_index << ","  
	    << rfj << ","
	    << afj << ","
	    << TargetResponse << ","
	    << AverageResponse << ","
	    << StandardDeviation << ","
	    << energy << ","
	    << miss_dline << endl;
	*/
	ifs.close();
}