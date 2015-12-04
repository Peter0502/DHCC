drop  table com_chrg_rate;
create table com_chrg_rate(
	cur_no varchar2(2),
	chrg_rate_code varchar2(3),
	beg_date number(9),
	end_date number(9),
	dc_code varchar2(4),
	name varchar2(20),
	type varchar2(1),
	val number(8,5),
	min_amt number(16,2),
	max_amt number(16,2) 
)tablespace users;
