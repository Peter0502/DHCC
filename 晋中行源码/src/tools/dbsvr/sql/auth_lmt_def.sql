drop  table auth_lmt_def;
create table auth_lmt_def(
	lmt_code varchar2(3),
	brf varchar2(20),
	min number(12,2),
	max number(12,2) 
)tablespace users;
