drop  table point_mst_hst;
create table point_mst_hst(
	ac_no varchar2(19),
	cif_type varchar2(1),
	used_point number(9),
	used_date number(9),
	brf varchar2(60) 
)tablespace users;
