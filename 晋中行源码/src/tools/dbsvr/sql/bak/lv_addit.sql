drop index lv_addit_idx;
drop  table lv_addit;
create table lv_addit(
	tx_date number(9),
	addid   varchar2(8),
	tagname varchar2(3),
	tagdata varchar2(2000) 
)tablespace users;
create	 unique index lv_addit_idx on lv_addit( tx_date,addid,tagname)tablespace indx;
