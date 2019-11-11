#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include <iomanip>
#include "export_csv.h"

using namespace std;

#define tREFW (64*800*1000)
//#define CheckNum 4
#define L_req 61
#define BI 4
#define tRRD 6
#define tFAW 32
#define tRC 38 
unsigned int CheckNum, demanded_refresh_overhead;

unsigned long PreNum, N_ap;
const unsigned int heuristic_rate = (unsigned int) BI;
unsigned long calc_tRFC_ap(unsigned long row_n);
unsigned long find_upper_mem_footprint(unsigned long row_n);

int main(int argc, char **argv)
{
	unsigned long UpperBound, subRef_rows, remain_rows, i;	
	double refresh_percent; 
	ExportResult *res_csv;
	eval_metrics res;
	if(argc != 6) {
		cout << "1) Please define the number of sub-refresh windows" << endl;
		cout << "2) Please give the demanded percentage of refresh overhead over one tREFW" << endl;
		cout << "3) Please give the filename of .CSV file where the result will be written to" << endl;
		cout << "4) Please decide whether to print out label at the first line of .CSV file or not. (Y/N)" << endl;
		cout << "5) Please decide whether to print out banswidt or not. (Y/N)" << endl;
		exit(1);
	}

	if(argv[4][0] == 'Y' ) {
		(argv[5][0] == 'Y') ? res_csv = new ExportResult(argv[3], (bool) true, (bool) true):
				      res_csv = new ExportResult(argv[3], (bool) true, (bool) false);
	}
	else if(argv[4][0] == 'N') {
		(argv[5][0] == 'Y') ? res_csv = new ExportResult(argv[3], (bool) false, (bool) true):
				      res_csv = new ExportResult(argv[3], (bool) false, (bool) false);
	}
	else {
		cout << "Undefined label_enable command was given" << endl;
		exit(1);
	}

	CheckNum = atoi(argv[1]); 
	PreNum = floor((tREFW / CheckNum) / L_req);
	//cout << "PreNum: " << PreNum << " requests" << endl;
	
	N_ap = PreNum * BI;
	//cout << "N^{ap}: " << N_ap << " rows" << endl;

	UpperBound = find_upper_mem_footprint((unsigned long) 6399596);
	//cout << "Upper bounded memory footprint: " << UpperBound << " rows" << endl;
	//cout << "Summation of N^{ap}_i for all i in {1...CheckNum}: " << N_ap*CheckNum << " rows" << endl;
	
 //-----------------------------------------------------------------------------
// Trial
	demanded_refresh_overhead = atoi(argv[2]);
	i = BI*5000; 
	unsigned long sub_refresh_cycle = tREFW / CheckNum;
	float percent_t = ((float) demanded_refresh_overhead) / 100;	
	float bound_t = floor(percent_t*sub_refresh_cycle);
	while((calc_tRFC_ap(i) <= bound_t)) {
	//	cout << "tRFC/sub_REFW: " << ((float)calc_tRFC_ap(i)) << "/" << sub_refresh_cycle << " = " << ((float)calc_tRFC_ap(i)*100)/(float) sub_refresh_cycle << "%" << endl;
		i += BI;	
	}
	//cout << "X: " << i << " rows" << endl;
	unsigned long A_t = sub_refresh_cycle - calc_tRFC_ap(i);
	unsigned long x_prime; x_prime = floor(A_t/L_req)*BI; 
	while(
		!(
			calc_tRFC_ap(x_prime) <= bound_t
		)
	) {
		x_prime -= BI;
	}
	//cout << "X_prime: " << x_prime << " rows" << endl;
	UpperBound = calc_tRFC_ap(x_prime);
	
	refresh_percent = (
			(double) calc_tRFC_ap(i)*CheckNum 
		)*100/(double) tREFW;
 //-----------------------------------------------------------------------------
	/*	
	i = UpperBound;
	do {
		subRef_rows = floor(i/CheckNum);
		remain_rows = i % CheckNum;  
		
		// Just to ensure no row is overlooked
		assert((subRef_rows*CheckNum + remain_rows) == i);
		
		UpperBound = calc_tRFC_ap(subRef_rows);
		refresh_percent = (
				(double) UpperBound*CheckNum + 
				(double) calc_tRFC_ap(remain_rows)
			)*100/(double) tREFW;
		//cout << "tRFC of one sub-refresh window: " << UpperBound << " cycles" << endl;
		
		//cout << "Totally " << i <<  " rows, Refresh / tREFW: " 
		//     << UpperBound*CheckNum + calc_tRFC(remain_rows) << " / " << tREFW << " = " 
	      	//     << refresh_percent << " %" << endl;
		
		i-=heuristic_rate;
	} while(refresh_percent >= demanded_refresh_overhead);
	*/

	// Upper bounding PreNum_i for all i in {1,CheckNum}
	
/*
	cout << "PreNum_i: " << PreNum << " requests, N^{ap}_i: " << N_ap << " rows" << endl;
	cout << "Totally " << i <<  " rows, Refresh / tREFW: " 
	     << UpperBound*CheckNum + calc_tRFC_ap(remain_rows) << " / " << tREFW << " = " 
      	     << refresh_percent << " %" << endl;
	cout << "Upper bounded # of requests in one tREFW: " 
	     << ceil(i/((int) BI)) << ", where (footprint/PreNum*CheckNum): " 
             << ceil(i/((int) BI))*100/(PreNum*CheckNum) << " %" << endl;
*/
	unsigned long temp = UpperBound*CheckNum + calc_tRFC_ap(remain_rows);
	//unsigned long temp_1 = floor(((tREFW/CheckNum) - UpperBound)/L_req)*L_req*CheckNum;
	unsigned long temp_1 = floor((float) x_prime / (float) BI)*CheckNum;
	double bandwidth_t = ((double) temp_1*64/(double) pow(2,20))*1000.0/64.0;
	//cout << "Refresh Overhead in one tREFW: " << temp
	//     << " cycles, Total response time for all requests: " << temp_1*L_req << " cycles" << endl
	//     << "(Ref+Tran)-tREFW: " << temp + temp_1 - tREFW << " cycles (should be < 1)" << endl;
	
	/*
	cout << "Minimum Bandwidth: " << bandwidth_t
	     << " MiB/s @ RefreshOverhead_" << fixed <<  setprecision(1) << refresh_percent << "% (" 
	     << i << " refreshed rows, " << i << "*" << CheckNum << " cycles)" << endl;
	*/

	// Export results to .CSV file
	res.bounded_rows = (unsigned long) i; 
	res.bi = (unsigned short) BI;
	res.bounded_rqst = (unsigned long) temp_1;
	res.refresh_overhead = (double) refresh_percent;
	res.utilisation = (double) ceil(i/((int) BI))*100/(PreNum*CheckNum);
	res.bandwidth = bandwidth_t;
	
	if(argv[5][0] == 'N')
		res_csv -> write_oneCase(&res);
	else
		res_csv -> write_oneCaseBW(&res);

	return 0;
}

unsigned long calc_tRFC_ap(unsigned long row_n)
{
 return (unsigned long) tRRD * (row_n - 1) + ceil((row_n - 1) / 4) * ((unsigned long) tFAW - 4 * (unsigned long) tRRD) + (unsigned long) tRC;
}

unsigned long find_upper_mem_footprint(unsigned long row_n)
{
	unsigned long subRef_rows = floor(row_n/CheckNum);
	unsigned long remain_rows = row_n % CheckNum;  
	unsigned long rfc_temp = calc_tRFC_ap(subRef_rows)*CheckNum + calc_tRFC_ap(remain_rows);
	
	// Just to ensure no row is overlooked
	assert((subRef_rows*CheckNum + remain_rows) == row_n);

	if(rfc_temp > tREFW) 
		return (row_n - BI);
	else if(rfc_temp == tREFW)
		return row_n;
	else {
		//cout << "Row: " << row_n << " tRFC: " << rfc_temp << " cycles tREFW: " << tREFW << " cycles" << endl;
		return find_upper_mem_footprint(row_n + BI);
	}
}
