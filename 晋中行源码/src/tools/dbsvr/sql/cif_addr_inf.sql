drop index cif_addr_inf_1;
drop  table cif_addr_inf;
create table cif_addr_inf(
	cif_no number(9),
	addr_seqn number(6),
	addr_type varchar2(2),
	country varchar2(3),
	addr varchar2(60),
	post_code varchar2(6) 
)tablespace users;
create	 unique index cif_addr_inf_1 on cif_addr_inf( cif_no,addr_seqn)tablespace indx;
