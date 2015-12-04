drop table cif_save_inf;
create table cif_save_inf(
	cif_no   number(9),
	name     varchar2(50),
	crt_date number(9),
	sts      varchar2(1),
	br_no    varchar2(5)
)tablespace users;
