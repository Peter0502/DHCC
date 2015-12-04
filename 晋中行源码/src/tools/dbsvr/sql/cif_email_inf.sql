drop index cif_email_inf_1;
drop  table cif_email_inf;
create table cif_email_inf(
	cif_no number(9),
	addr_seqn number(6),
	addr_type varchar2(2),
	email varchar2(20),
	link_man varchar2(50) 
)tablespace users;
create	 unique index cif_email_inf_1 on cif_email_inf( cif_no,addr_seqn)tablespace indx;
