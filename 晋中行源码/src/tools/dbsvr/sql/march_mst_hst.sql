drop index march_mst_hst_1;
drop  table march_mst_hst;
create table march_mst_hst(
	area_code varchar2(4),
	march_code varchar2(15),
	ac_no varchar2(20),
	mch_name varchar2(40),
	tx_date number(9),
	tx_cnt number(9),
	tx_amt number(16,2),
	fee_amt number(16,2),
	fee_rate number(12,6),
	real_amt number(16,2),
	acct_date number(9),
	deal_sign varchar2(1),
	deal_msg varchar2(40),
	opn_br_no varchar2(5),
	cz_pt_trace_no number(9),
	cz_pt_date number(9),
	xf_ac_no varchar2(24),
	trace_no number(9),
	pt_trace_no number(9),
	pt_date number(9),
	dc_ind varchar2(1),
	cz_ind varchar2(1),
	ysq_ind varchar2(1),
	cx_ind varchar2(1)		
)tablespace users;
create	 unique index march_mst_hst_1 on march_mst_hst( march_code,tx_date,trace_no)tablespace indx;

SQL*Plus: Release 9.2.0.1.0 - Production on ÐÇÆÚÈÕ 12ÔÂ 20 21:57:30 2009

Copyright (c) 1982, 2002, Oracle Corporation.  All rights reserved.


Connected to:
Oracle9i Enterprise Edition Release 9.2.0.1.0 - 64bit Production
With the Partitioning, OLAP and Oracle Data Mining options
JServer Release 9.2.0.1.0 - Production

SQL> 
SQL> SQL> SQL> SQL> SQL> SQL> SQL> SQL> SQL> SQL> SQL> SQL> SQL> SQL> 
SQL> Disconnected from Oracle9i Enterprise Edition Release 9.2.0.1.0 - 64bit Production
With the Partitioning, OLAP and Oracle Data Mining options
JServer Release 9.2.0.1.0 - Production
