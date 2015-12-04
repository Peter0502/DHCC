drop index auth_chk_def_1;
drop  table auth_chk_def;
create table auth_chk_def(
	tx_code varchar2(4),
	seqn number(6),
	data_code varchar2(4),
	mo varchar2(20) 
)tablespace users;
create	 unique index auth_chk_def_1 on auth_chk_def( tx_code,seqn)tablespace indx;
