drop  table com_def_chrg;
create table com_def_chrg(
	tx_code varchar2(4),
	seqn number(6),
	chrg_rate_code varchar2(3),
	chrg_ac_no varchar2(4),
	chrg_mode varchar2(1),
	get_ind varchar2(1),
	cond varchar2(40),
	amt_no varchar2(40),
	mo varchar2(20) 
)tablespace users;
