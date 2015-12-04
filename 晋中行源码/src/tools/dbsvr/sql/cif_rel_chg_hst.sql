drop  table cif_rel_chg_hst;
create table cif_rel_chg_hst(
	cif_no number(9),
	chg_tablename varchar2(20),
	chg_columnname varchar2(20),
	chg_date number(9),
	chg_time number(9),
	bef_content varchar2(40),
	aft_content varchar2(40),
	clerk_no varchar2(5),
	rmk varchar2(40) 
)tablespace users;
