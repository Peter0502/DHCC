drop  table com_tel_connect;
create table com_tel_connect(
	tel varchar2(4),
	tw_tel varchar2(4),
	amt number(16,2),
	br_no varchar2(5),
	tx_date number(9),
	cur_no varchar2(2),
	trace_no number(9),
	ind varchar2(1) 
)tablespace users;
