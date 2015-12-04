drop  table cif_mger_rel;
create table cif_mger_rel(
	cif_no number(9),
	mang varchar2(4),
	mang_type varchar2(1),
	crt_date number(9),
	canl_date number(9) 
)tablespace users;
