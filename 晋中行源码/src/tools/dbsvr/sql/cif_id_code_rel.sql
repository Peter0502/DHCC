drop index cif_id_code_rel_1;
drop index cif_id_code_rel_2;
drop  table cif_id_code_rel;
create table cif_id_code_rel(
	cif_no number(9),
	id_type varchar2(1),
	id_no varchar2(20) 
)tablespace users;
create	 unique index cif_id_code_rel_1 on cif_id_code_rel( id_type,id_no)tablespace indx;
create	 index cif_id_code_rel_2 on cif_id_code_rel( cif_no)tablespace indx;
