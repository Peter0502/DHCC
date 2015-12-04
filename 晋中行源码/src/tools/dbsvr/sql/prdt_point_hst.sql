drop  table prdt_point_hst;
create table prdt_point_hst(
	prdt_no varchar2(3),
	used_point number(9),
	used_date number(9),
	brf varchar2(60) 
)tablespace users;
