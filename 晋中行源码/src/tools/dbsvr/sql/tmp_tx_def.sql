drop  table tmp_tx_def;
create table tmp_tx_def(
	tx_code varchar2(4),
	tx_name varchar2(20),
	bit_map varchar2(128),
	log_ind varchar2(1),
	tx_type varchar2(1),
	wrk_sts varchar2(1) 
)tablespace users;
