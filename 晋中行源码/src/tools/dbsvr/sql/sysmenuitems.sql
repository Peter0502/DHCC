drop index meniidx;
drop  table sysmenuitems;
create table sysmenuitems(
	imenuname varchar2(18),
	itemnum number(9),
	mtext varchar2(60),
	mtype varchar2(1),
	progname varchar2(60) 
)tablespace users;
create	 unique index meniidx on sysmenuitems( imenuname,itemnum)tablespace indx;
