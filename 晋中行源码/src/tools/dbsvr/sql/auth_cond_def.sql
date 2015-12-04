drop  table auth_cond_def;
create table auth_cond_def(
	cond_code varchar2(3),
	tx_code varchar2(4),
	seqn number(6),
	cond_mo varchar2(40) 
)tablespace users;
