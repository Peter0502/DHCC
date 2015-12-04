drop  table tx_sub_def_tmp;
create table tx_sub_def_tmp(
	sub_tx_code varchar2(4),
	sub_tx_name varchar2(20),
	tx_func varchar2(30),
	acct_ind varchar2(1),
	chnl_ind varchar2(1) 
)tablespace users;
