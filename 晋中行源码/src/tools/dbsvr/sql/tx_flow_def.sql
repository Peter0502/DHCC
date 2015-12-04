drop index tx_flow_def_1;
drop  table tx_flow_def;
create table tx_flow_def(
	tx_code varchar2(4),
	flow_code number(6),
	sub_tx_code varchar2(4),
	sub_tx_rslt varchar2(200) 
)tablespace users;
create	 unique index tx_flow_def_1 on tx_flow_def( tx_code,flow_code)tablespace indx;
