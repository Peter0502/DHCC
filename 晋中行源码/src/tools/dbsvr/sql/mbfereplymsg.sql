drop index mbfereplymsg_idx;
drop  table mbfereplymsg;
create table mbfereplymsg(
	rplymsgno varchar2(8),
	rplydate number(9),
	origquerymsgno varchar2(8),
	odficenter varchar2(4),
	isdficode varchar2(12),
	rplybkcode varchar2(12),
	rdficenter varchar2(4),
	origqueryisdficode varchar2(12),
	origquerybkcode varchar2(12),
	origquerydate number(9),
	origconsigndate number(9),
	origodficode varchar2(12),
	origrdficode varchar2(12),
	origtxtype varchar2(1),
	origtxssno varchar2(8),
	origamount number(15,2),
	rplycontent varchar2(255),
	rplyuser varchar2(4),
	procstate varchar2(2),
	statetime varchar2(14),
	errcode varchar2(8),
	errdesc varchar2(60),
	printno number(9),
	rplysource varchar2(2) 
)tablespace users;
create	 unique index mbfereplymsg_idx on mbfereplymsg( rplymsgno,rplydate,rplybkcode)tablespace indx;
