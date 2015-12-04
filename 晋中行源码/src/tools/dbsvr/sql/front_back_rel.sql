drop index front_back_rel_1;
drop  table front_back_rel;
create table front_back_rel(
	f_txcode varchar2(4),
	b_txcode varchar2(4),
	tx_name  varchar2(20),
	filler   varchar2(20) 
)tablespace users;
create	 unique index front_back_rel_1 on front_back_rel( f_txcode)tablespace indx;
