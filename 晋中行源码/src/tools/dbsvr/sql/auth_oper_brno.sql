drop  table auth_oper_brno;
create table auth_oper_brno(
	oper_code varchar2(5),
	oper_name varchar2(20),
	dw_brno varchar2(5),
	up_brno varchar2(5) 
)tablespace users;
