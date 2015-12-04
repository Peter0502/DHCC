drop  table auth_def;
create table auth_def(
	tx_code varchar2(4),
	seqn number(6),
	cond_code varchar2(3),
	lvl varchar2(11),
	chk_ind varchar2(2),
	flow_code varchar2(3),
	mo varchar2(20) 
)tablespace users;
