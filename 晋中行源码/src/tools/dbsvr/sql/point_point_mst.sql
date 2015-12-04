drop  table point_point_mst;
create table point_point_mst(
	ac_no varchar2(19),
	cif_type varchar2(1),
	beg_date number(9),
	point number(9),
	yt_point number(9),
	used_point number(9) 
)tablespace users;
