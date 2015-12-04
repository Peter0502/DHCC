drop  table tx_flow_def_tmp;
create table tx_flow_def_tmp(
	tx_code varchar2(4),
	flow_code number(6),
	sub_tx_code varchar2(4),
	sub_tx_rslt varchar2(200) 
)tablespace users;
