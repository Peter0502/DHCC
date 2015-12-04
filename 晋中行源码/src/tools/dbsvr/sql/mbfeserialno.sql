drop index mbfeserialno_idx;
drop  table mbfeserialno;
create table mbfeserialno(
	serialnocode varchar2(2),
	serialnoname varchar2(60),
	serialnoval number(9),
	serialnolen number(9) 
)tablespace users;
create	 unique index mbfeserialno_idx on mbfeserialno( serialnocode)tablespace indx;
