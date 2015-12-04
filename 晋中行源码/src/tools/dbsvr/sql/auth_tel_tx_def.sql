drop  table auth_tel_tx_def;
create table auth_tel_tx_def(
	tel varchar2(4),
	br_no varchar2(5),
	tx_code varchar2(4),
	auth_code varchar2(3) 
)tablespace users;
