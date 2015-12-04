drop index menu_1;
drop  table menu;
create table menu(
	code varchar2(4),
	title varchar2(20),
	lvl varchar2(8),
	maincode varchar2(4),
	subcode varchar2(4),
	prog varchar2(12),
	dspcode varchar2(4) 
)tablespace users;
create	 unique index menu_1 on menu( code)tablespace indx;
