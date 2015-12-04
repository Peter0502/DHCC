#ifndef __hvstat_CH__
#define __hvstat_CH__
struct hvstat_c{
	char      rowid[20];
	long      tx_date;
	char      stat[2];
	char      chflag[2];
	char      message[61];
	char      filler1[31];
	char      filler2[31];
};
#endif 
