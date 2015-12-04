#ifndef __mo_po_qry_CH__
#define __mo_po_qry_CH__
struct mo_po_qry_c{
	char      rowid[20];
	char      po_no[17];
	char      qry_typ[2];
	long      tx_date;
	char      brf[201];
	char      sts[2];
	char      tel[6];
	char      br_no[6];
};
#endif 
