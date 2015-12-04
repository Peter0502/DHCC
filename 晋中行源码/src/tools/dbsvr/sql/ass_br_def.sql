drop index ass_br_def_1;
drop  table ass_br_def;
create table ass_br_def(
	br_no varchar2(5),
	name varchar2(40),
	sub_br_no varchar2(5),
	beg_date number(9),
	end_date number(9),
	sts varchar2(1) 
)tablespace users;
create   unique index ass_br_def_1 on ass_br_def(sub_br_no)tablespace indx;
