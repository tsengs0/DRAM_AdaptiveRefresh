#include <iostream>
#include <cstdio>
#include <cstdlib>
#include<fstream> 
#include<string>
#include<sstream>

using namespace std;

enum {
	BOUNDED_ROWS  	  = 0, // Column 1 in .CSV File => Upper bounded available distinct rows
	BIs		  = 1, // Column 2 in .CSV File => # of interleaving banks
	BOUNDED_REQUESTS  = 2, // Column 3 in .CSV File => Upper bounded available requests
	REFRESH_OVERHEAD  = 3, // Column 4 in .CSV File => Refresh overhead over one tREFW
	UTILISATION       = 4  // Column 5 in .CSV File => Utilisation
};

typedef struct Eval_Metrics {
	unsigned long bounded_rows; // rows
	unsigned short bi;          // rows
	unsigned long bounded_rqst; // requests
	double refresh_overhead;    // %
	double utilisation;         // %
	double bandwidth;	    // MiB/s
} eval_metrics;

class ExportResult {
	private:
	eval_metrics result;
	ofstream *ofs;

	public:
	ExportResult(char *filename, bool label_en, bool printBW_en);
	//~ExportResult(void);
	void write_oneCase(eval_metrics *res);
	void write_oneCaseBW(eval_metrics *res);
};

ExportResult::ExportResult(char *filename, bool label_en, bool printBW_en)
{
	ofs = new ofstream(filename, ios::out | ios::app);
	if(label_en == true && printBW_en == false)
		*ofs << "UpperBounded #rows, BI, UpperBounded #requests, Refresh Overhead (%), MemUtilisation (%)" << endl;
	else if(label_en == true && printBW_en == true)
		*ofs << "BI, UpperBounded #requests, Refresh Overhead (%), Bandwidth (MiB/s)" << endl;
}

void ExportResult::write_oneCase(eval_metrics *res)
{
	*ofs << res -> bounded_rows     << ","
	    << res -> bi    	        << ","
	    << res -> bounded_rqst      << ","
	    << res -> refresh_overhead  << ","
	    << res -> utilisation       << endl;  
	//ofs -> close();
}

void ExportResult::write_oneCaseBW(eval_metrics *res)
{
	*ofs << res -> bi    	         << ","
	     << res -> bounded_rqst      << ","
	     << res -> refresh_overhead  << ","
	     << res -> bandwidth	 << endl;
	//ofs -> close();
}
