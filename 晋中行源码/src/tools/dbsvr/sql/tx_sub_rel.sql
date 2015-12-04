drop index tx_sub_rel_1;
drop  table tx_sub_rel;
create table tx_sub_rel(
	tx_code varchar2(4),
	in_sub_tx varchar2(4),
	out_ind varchar2(1),
	out_data varchar2(40),
	in_data varchar2(4),
	memo varchar2(10) 
)tablespace users;
create	 unique index tx_sub_rel_1 on tx_sub_rel( tx_code,in_sub_tx,in_data)tablespace indx;
