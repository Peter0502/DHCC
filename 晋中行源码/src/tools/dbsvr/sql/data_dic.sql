drop  table data_dic;
create table data_dic(
	data_code varchar2(4),
	data_type varchar2(1),
	data_leng number(6),
	data_dec number(6),
	data_name varchar2(20),
	bit_seq number(6) 
)tablespace users;
