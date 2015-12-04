drop index mbfereturnrply_idx;
drop  table mbfereturnrply;
create table mbfereturnrply(
	consigndate number(9),
	origrtnappdate number(9),
	origrtnappno varchar2(3),
	odficenter varchar2(4),
	isdficode varchar2(12),
	rtnrplybkcode varchar2(12),
	rdficenter varchar2(4),
	osdficode varchar2(12),
	rdficode varchar2(12),
	origconsigndate number(9),
	origtxtype varchar2(1),
	origtxssno varchar2(8),
	origamount number(15,2),
	rtnrplyuser varchar2(4),
	rtnrplyrus varchar2(1),
	procstate varchar2(2),
	statetime varchar2(14),
	errcode varchar2(8),
	errdesc varchar2(60),
	printno number(9),
	rtnrplysource varchar2(2),
	strinfo varchar2(60) 
)tablespace users;
create	 unique index mbfereturnrply_idx on mbfereturnrply( origrtnappdate,origrtnappno,rdficode)tablespace indx;
