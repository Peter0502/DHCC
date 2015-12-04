drop  table com_chrg_hst;
create table com_chrg_hst(
	tx_date number(9),
	trace_no number(9),
	ac_no varchar2(19),
	tx_code varchar2(4),
	chrg_rate_code varchar2(3),
	name varchar2(30),
	chrg_amt number(16,2),
	tx_amt number(16,2),
	sts varchar2(1) 
)tablespace users;
