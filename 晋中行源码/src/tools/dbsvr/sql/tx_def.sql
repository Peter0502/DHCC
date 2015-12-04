drop index tx_def_1;
drop  table tx_def;
create table tx_def(
	tx_code varchar2(4),
	tx_name varchar2(20),
	bit_map varchar2(128),
	log_ind varchar2(1),
	tx_type varchar2(1),
	wrk_sts varchar2(1) 
)tablespace users;
create	 unique index tx_def_1 on tx_def( tx_code)tablespace indx;
