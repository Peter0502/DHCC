drop index tx_sub_def_1;
drop  table tx_sub_def;
create table tx_sub_def(
	sub_tx_code varchar2(4),
	sub_tx_name varchar2(20),
	tx_func varchar2(30),
	acct_ind varchar2(1),
	chnl_ind varchar2(1) 
)tablespace users;
create	 unique index tx_sub_def_1 on tx_sub_def( sub_tx_code)tablespace indx;
