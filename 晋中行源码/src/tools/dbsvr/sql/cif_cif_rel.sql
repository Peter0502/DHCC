drop  table cif_cif_rel;
create table cif_cif_rel(
	cif_no number(9),
	rel_cid_no number(9),
	rel_code varchar2(3),
	rel_sts varchar2(1),
	crt_date number(9),
	cls_date number(9) 
)tablespace users;
