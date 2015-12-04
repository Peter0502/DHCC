drop index auth_cond_1;
drop  table auth_cond;
create table auth_cond(
	type varchar2(1),
	no number(6),
	tx_code varchar2(4),
	seqn number(6),
	mo varchar2(20),
	cond varchar2(100) 
)tablespace users;
create	 index auth_cond_1 on auth_cond( tx_code,seqn)tablespace indx;
