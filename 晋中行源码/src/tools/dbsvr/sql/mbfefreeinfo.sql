drop index mbfefreeinfo_idx;
drop  table mbfefreeinfo;
create table mbfefreeinfo(
	msssno varchar2(8),
	consigndate number(9),
	odficenter varchar2(4),
	isdficode varchar2(12),
	odficode varchar2(12),
	rdficenter varchar2(4),
	osdficode varchar2(12),
	rdficode varchar2(12),
	procstate varchar2(2),
	statetime varchar2(14),
	errcode varchar2(8),
	errdesc varchar2(60),
	putuser varchar2(4),
	infosource varchar2(2),
	infomation varchar2(255),
	printno number(9) 
)tablespace users;
create	 unique index mbfefreeinfo_idx on mbfefreeinfo( msssno,consigndate,infosource)tablespace indx;
