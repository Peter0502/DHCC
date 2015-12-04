drop index auth_item_1;
drop  table auth_item;
create table auth_item(
	tx_code varchar2(4),
	seqn number(6),
	code_ac varchar2(4),
	code_seqn varchar2(4),
	code_amt varchar2(4),
	code_dc varchar2(4),
	code_ct varchar2(4),
	code_xx varchar2(4),
	code_yy varchar2(4),
	code_zz varchar2(4),
	ind varchar2(4),
	mo varchar2(20) 
)tablespace users;
create	 unique index auth_item_1 on auth_item( tx_code,seqn)tablespace indx;
