drop  table ag_unitacctmap;
create table ag_unitacctmap(
	br_no varchar2(7),
	unit_no varchar2(4),
	acct_type varchar2(1),
	bank_acct_no varchar2(25),
	stat varchar2(1) 
)tablespace users;
