drop  table auth_auth_lmt;
create table auth_auth_lmt(
	auth_code varchar2(3),
	min_amt number(12,2),
	max_amt number(12,2),
	mo varchar2(20) 
)tablespace users;
