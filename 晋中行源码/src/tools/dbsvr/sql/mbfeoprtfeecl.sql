drop index mbfeoprtfeecl_idx;
drop  table mbfeoprtfeecl;
create table mbfeoprtfeecl(
	consigndate number(9),
	rdficenter varchar2(4),
	feebkcode varchar2(12),
	feebgdate number(9),
	feeeddate number(9),
	feeammunt number(15,2),
	sapsrate number(7,4),
	bktprate number(7,4),
	mothrate number(7,4),
	hvpfeeamt number(15,2),
	hvpfeecnt number(9),
	singlefeeamt number(15,2),
	singlefeecnt number(9),
	infofeeamt number(15,2),
	infofeecnt number(9),
	feeacno varchar2(19),
	printno number(9) 
)tablespace users;
create	 unique index mbfeoprtfeecl_idx on mbfeoprtfeecl( consigndate)tablespace indx;
