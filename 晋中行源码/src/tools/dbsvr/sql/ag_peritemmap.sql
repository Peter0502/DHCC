drop index ind_110_2;
drop  table ag_peritemmap;
create table ag_peritemmap(
	unit_no varchar2(4),
	item_no varchar2(20),
	name varchar2(50),
	id_type varchar2(1),
	id_no varchar2(20),
	ac_no varchar2(19),
	remark varchar2(50),
	bgn_use_date number(9),
	bat_flag varchar2(1),
	stat varchar2(1),
	opn_date number(9),
	opn_tel varchar2(4),
	cls_date number(9),
	cls_tel varchar2(4) 
)tablespace users;
create	 unique index ind_110_2 on ag_peritemmap( unit_no,item_no)tablespace indx;
