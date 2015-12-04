drop index sysmenidx;
drop  table sysmenus;
create table sysmenus(
	menuname varchar2(18),
	title varchar2(60) 
)tablespace users;
create	 unique index sysmenidx on sysmenus( menuname)tablespace indx;
