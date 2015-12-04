drop  table cif_blklst_rel;
create table cif_blklst_rel(
	cif_no number(9),
	blalist_origin varchar2(1),
	relation_sts varchar2(1),
	rmk varchar2(40),
	crt_date number(9),
	cls_date number(9) 
)tablespace users;
