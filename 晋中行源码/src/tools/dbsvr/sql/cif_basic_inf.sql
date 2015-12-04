drop index cif_basic_inf_1;
drop  table cif_basic_inf;
create table cif_basic_inf(
	cif_no number(9),
	type varchar2(1),
	name varchar2(50),
	local varchar2(1),
	poten varchar2(1),
	crt_date number(9),
	crt_time number(9),
	lvl varchar2(1),
	shareholder_ind varchar2(1),
	cif_seqn number(9),
	sts varchar2(1) 
)tablespace users;
create	 unique index cif_basic_inf_1 on cif_basic_inf( cif_no)tablespace indx;
