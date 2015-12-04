drop  table tx_log;
create table tx_log(
	trace_no number(9),
	tx_code varchar2(4),
	tx_br_no varchar2(5),
	bit_map varchar2(128),
	tx_pack varchar2(1024) 
)tablespace users;
