drop index mbfequerymsg_idx;
drop  table mbfequerymsg;
create table mbfequerymsg(
	querymsgno varchar2(8),
	querydate number(9),
	odficenter varchar2(4),
	isdficode varchar2(12),
	querybkcode varchar2(12),
	rdficenter varchar2(4),
	osdficode varchar2(12),
	rplybkcode varchar2(12),
	origconsigndate number(9),
	origodficode varchar2(12),
	origrdficode varchar2(12),
	origtxtype varchar2(1),
	origtxssno varchar2(8),
	origamount number(15,2),
	querycontent varchar2(255),
	queryuser varchar2(4),
	procstate varchar2(2),
	statetime varchar2(14),
	errcode varchar2(8),
	errdesc varchar2(60),
	printno number(9),
	querysource varchar2(2) 
)tablespace users;
create	 unique index mbfequerymsg_idx on mbfequerymsg( querymsgno,querydate,querybkcode)tablespace indx;
