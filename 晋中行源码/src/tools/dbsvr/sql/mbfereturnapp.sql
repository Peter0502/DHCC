drop index mbfereturnapp_idx;
drop  table mbfereturnapp;
create table mbfereturnapp(
	rtnappno varchar2(3),
	consigndate number(9),
	odficenter varchar2(4),
	isdficode varchar2(12),
	odficode varchar2(12),
	rdficenter varchar2(4),
	osdficode varchar2(12),
	rdficode varchar2(12),
	origconsigndate number(9),
	origtxtype varchar2(1),
	origtxssno varchar2(8),
	origamount number(15,2),
	appuser varchar2(4),
	procstate varchar2(2),
	statetime varchar2(14),
	errcode varchar2(8),
	errdesc varchar2(60),
	printno number(9),
	rtnappsource varchar2(2),
	strinfo varchar2(60) 
)tablespace users;
create	 unique index mbfereturnapp_idx on mbfereturnapp( rtnappno,consigndate,odficode)tablespace indx;
