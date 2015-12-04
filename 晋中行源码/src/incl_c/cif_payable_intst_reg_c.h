#ifndef __cif_payable_intst_reg_CH__
#define __cif_payable_intst_reg_CH__
struct cif_payable_intst_reg_c{
	char      rowid[20];
	long      ac_id;
	long      ac_seqn;
	long      cif_no;
	char      ac_no[33];
	char      name[61];
	char      opn_br_no[6];
	char      prdt_no[4];
	long      beg_date;
	long      end_date;
	double    bal;
	double    intst_acm;
	double    rate;
	double    intst;
	double    intst_tax;
	long      tx_date;
	long      trace_no;
};
#endif 
