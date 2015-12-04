drop index remark_index1;
drop  table remark_ctl;
create table remark_ctl(
	br_no varchar2(5),
	ac_no varchar2(24),
	ac_id number(9),
	ac_seqn number(6),
	rem_code varchar2(10),
	amt number(16,2),
	rem varchar2(50),
	tx_date number(9),
	tx_tel varchar2(4),
	lst_date number(9),
	lst_tel varchar2(4),
	rem_sts varchar2(1) 
)tablespace users;
create	 unique index remark_index1 on remark_ctl( ac_no,ac_seqn,rem_code)tablespace indx;
