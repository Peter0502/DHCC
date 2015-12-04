drop  table fin_acct;
create table fin_acct(
	tx_date number(9),
	fin_date number(9),
	ac_no varchar2(21),
	amt number(16,2),
	dc_flag varchar2(1),
	bal number(16,2),
	acct_flag varchar2(1) 
)tablespace users;
