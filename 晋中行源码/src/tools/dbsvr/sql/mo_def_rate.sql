drop  table mo_def_rate;
create table mo_def_rate(
	chrg_code varchar2(3),
	chrg_type number(6),
	min_amt number(16,2),
	max_amt number(16,2),
	chrg_rate_no varchar2(3) 
)tablespace users;
