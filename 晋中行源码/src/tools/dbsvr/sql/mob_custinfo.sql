drop  table mob_custinfo;
create table mob_custinfo(
	cust_no varchar2(10),
	cust_name varchar2(50),
	mobile varchar2(15),
	"group" number(9),
	passwd varchar2(6) 
)tablespace users;
