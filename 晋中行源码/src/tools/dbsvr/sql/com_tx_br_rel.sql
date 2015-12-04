drop index com_tx_br_rel_1;
drop  table com_tx_br_rel;
create table com_tx_br_rel(
	rel_type number(6),
	br_no varchar2(5),
	up_br_no varchar2(5) 
)tablespace users;
create	 index com_tx_br_rel_1 on com_tx_br_rel( rel_type,br_no)tablespace indx;
