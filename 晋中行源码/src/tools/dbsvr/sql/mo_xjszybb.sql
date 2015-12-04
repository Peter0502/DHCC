drop index mo_xjszybb_1;
drop  table mo_xjszybb;
create table mo_xjszybb(
	br_no varchar2(5),
	target_no varchar2(20),
	tx_date number(9),
	amt number(16,2) 
)tablespace users;
create	 index mo_xjszybb_1 on mo_xjszybb( br_no,target_no,tx_date)tablespace indx;
