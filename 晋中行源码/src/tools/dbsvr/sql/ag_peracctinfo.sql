drop  table ag_peracctinfo;
create table ag_peracctinfo(
	per_no varchar2(12),
	per_name varchar2(50),
	per_stat varchar2(1),
	acct_type varchar2(1),
	bank_acct_no varchar2(25),
	per_pwd varchar2(6),
	opn_br_no varchar2(7),
	opn_date varchar2(8),
	opn_tel varchar2(4),
	cls_date varchar2(8),
	cls_tel varchar2(4) 
)tablespace users;
