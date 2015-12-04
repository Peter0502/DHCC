drop  table mo_ibl_ret;
create table mo_ibl_ret(
	pact_no varchar2(20),
	tx_br_no varchar2(5),
	ret_amt number(16,2),
	ret_rate number(16,2),
	tx_date number(9),
	trace_no number(9),
	sts varchar2(1),
	tel varchar2(4),
	chk varchar2(4),
	auth varchar2(4) 
)tablespace users;
