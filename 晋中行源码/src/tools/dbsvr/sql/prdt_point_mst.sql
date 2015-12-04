drop  table prdt_point_mst;
create table prdt_point_mst(
	prdt_no varchar2(3),
	beg_date number(9),
	point number(9),
	yt_point number(9),
	used_point number(9) 
)tablespace users;
