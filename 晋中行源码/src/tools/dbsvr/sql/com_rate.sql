drop index com_rate_1;
drop  table com_rate;
create table com_rate(
	cur_no varchar2(2),
	rate_code varchar2(3),
	name varchar2(50),
	beg_date number(9),
	end_date number(9),
	rate_val number(8,5),
	rate_type number(6),
	rate_unit number(9),
	fl_rate_code varchar2(3),
	ref_rate_code varchar2(3),
	ref_ratio number(8,5) 
)tablespace users;
create	 index com_rate_1 on com_rate( cur_no,rate_code,beg_date,end_date)tablespace indx;
