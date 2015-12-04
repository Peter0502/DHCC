drop  table cash_book;
create table cash_book(
	trace_no number(9),
	tx_date number(9),
	br_no varchar2(5),
	tel varchar2(4),
	tw_br_no varchar2(5),
	tw_tel varchar2(4),
	cur_no varchar2(2),
	amt number(16,2),
	ind varchar2(1),
	wrk_no number(9),
	out_no number(9) 
)tablespace users;
