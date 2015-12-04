drop  table cif_inf_chg_log;
create table cif_inf_chg_log(
	cif_no number(9),
	chg_tablename varchar2(20),
	chg_columnname varchar2(20),
	chg_date number(9),
	chg_time number(9),
	bef_content varchar2(50),
	aft_content varchar2(50),
	clerk_no varchar2(5),
	memo varchar2(40) 
)tablespace users;
