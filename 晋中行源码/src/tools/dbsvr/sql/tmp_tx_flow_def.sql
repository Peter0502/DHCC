drop  table tmp_tx_flow_def;
create table tmp_tx_flow_def(
	tx_code varchar2(4),
	flow_code number(6),
	sub_tx_code varchar2(4),
	sub_tx_rslt varchar2(200) 
)tablespace users;
