drop  table cif_prdt_rel;
create table cif_prdt_rel(
	cif_no number(9),
	prdt_code varchar2(3),
	ac_id number(9),
	relation_no varchar2(2),
	relation_flag varchar2(1),
	addr_seri number(6),
	crt_date number(9),
	cls_date number(9) 
)tablespace users;
