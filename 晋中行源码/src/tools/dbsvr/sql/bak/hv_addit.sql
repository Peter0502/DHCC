drop index hv_addit_idx;
drop  table hv_addit;
create table hv_addit(
	tx_date number(9),
	addid varchar2(8),
	tagname varchar2(3),
	tagdata varchar2(2000) 
)tablespace users;
create	 unique index hv_addit_idx on hv_addit( tx_date,addid,tagname)tablespace indx;
