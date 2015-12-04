drop  table mbfeoverdraftcl;
create table mbfeoverdraftcl(
	consigndate number(9),
	rdficenter varchar2(4),
	osdficode varchar2(12),
	rdficode varchar2(12),
	accrualamnt number(22,2),
	accruacno varchar2(19),
	printno number(9) 
)tablespace users;
