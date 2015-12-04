drop  table cif_crdt_inf;
create table cif_crdt_inf(
	credit_no varchar2(10),
	cur_no varchar2(2),
	bal number(16,2),
	amt number(16,2),
	cyc_ind varchar2(1) 
)tablespace users;
