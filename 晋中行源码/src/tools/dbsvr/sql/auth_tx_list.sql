drop index auth_tx_list_1;
drop  table auth_tx_list;
create table auth_tx_list(
	tx_type number(6),
	menu_code varchar2(4) 
)tablespace users;
create	 unique index auth_tx_list_1 on auth_tx_list( tx_type,menu_code)tablespace indx;
