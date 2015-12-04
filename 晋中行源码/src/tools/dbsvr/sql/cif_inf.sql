drop index cif_inf_1;
drop table cif_inf;
create table cif_inf(
	cif_no number(9),
	name   varchar2(60),
	crt_date number(9),
	sts    varchar2(1),
	br_no  varchar2(5)
)tablespace users;
create unique index cif_inf_1 on cif_inf(cif_no) tablespace indx;


