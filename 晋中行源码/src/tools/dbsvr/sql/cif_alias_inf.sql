drop index cif_alias_inf_1;
drop  table cif_alias_inf;
create table cif_alias_inf(
	cif_no number(9),
	alias_seqn number(6),
	alias varchar2(50) 
)tablespace users;
create	 unique index cif_alias_inf_1 on cif_alias_inf( cif_no,alias_seqn)tablespace indx;
