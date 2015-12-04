drop  table auth_inf;
create table auth_inf(
	auth_no number(9),
	br_no varchar2(5),
	tel varchar2(4),
	mtr_date number(9),
	tx_code varchar2(4),
	amt number(16,2),
	ac_no varchar2(20),
	auth_tel varchar2(4),
	auth_br_no varchar2(5),
	auth_date number(9),
	sts varchar2(1),
	use_date number(9),
	use_trace_no number(9),
	use_amt number(16,2),
	use_tel varchar2(4) 
)tablespace users;
