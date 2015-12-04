drop  table cif_crdt_use_det;
create table cif_crdt_use_det(
	cif_no number(9),
	credit_no varchar2(10),
	wrkdate number(9),
	trace_no number(9),
	amt number(16,2),
	memo varchar2(40),
	flag varchar2(1) 
)tablespace users;
