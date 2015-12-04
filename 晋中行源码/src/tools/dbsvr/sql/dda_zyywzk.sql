drop index dda_zyywzk_1;
drop  table dda_zyywzk;
create table dda_zyywzk(
	inst_no varchar2(7),
	hz_bank_no varchar2(7),
	ywtjbm varchar2(3),
	item_no varchar2(5),
	lst_ybal number(22,6),
	lst_sbal number(22,6),
	lst_mbal number(22,6),
	lst_dbal number(22,6),
	balance number(22,6),
	tdy_accu number(22,6),
	days number(9),
	year varchar2(4),
	last_accu number(22,6) 
)tablespace users;
create	 unique index dda_zyywzk_1 on dda_zyywzk( inst_no,ywtjbm,item_no)tablespace indx;
