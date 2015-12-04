drop  table tx_dc_rel;
create table tx_dc_rel(
	sub_tx_code varchar2(4),
	dc_code varchar2(4),
	entry_code varchar2(6) 
)tablespace users;
