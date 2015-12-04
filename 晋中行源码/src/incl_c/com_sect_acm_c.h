#ifndef __com_sect_acm_CH__
#define __com_sect_acm_CH__
struct com_sect_acm_c{
	char      rowid[20];
	long      ac_id;
	long      ac_seqn;
	long      beg_date;
	long      end_date;
	double    rate;
	double    intst_acm;
	char      intst_type[2];
	char      sts[2];
};
#endif 
