drop index mbfe_hang_idx;
drop  table mbfe_hang;
create table mbfe_hang(
	consigndate number(9),
	txssno varchar2(8),
	odficode varchar2(12),
	hang_reply varchar2(4) 
)tablespace users;
create	 unique index mbfe_hang_idx on mbfe_hang( txssno,odficode,consigndate)tablespace indx;
