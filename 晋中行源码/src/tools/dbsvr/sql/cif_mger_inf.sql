drop  table cif_mger_inf;
create table cif_mger_inf(
	mang varchar2(4),
	mang_name varchar2(20),
	crt_date number(9),
	mtr_date number(9),
	mang_type varchar2(1),
	center varchar2(4),
	phone varchar2(20) 
)tablespace users;
