drop index mbfeoprtcancel_idx;
drop  table mbfeoprtcancel;
create table mbfeoprtcancel(
	cancelno varchar2(3),
	consigndate number(9),
	applyuser varchar2(4),
	grantuser varchar2(4),
	origconsigndate number(9),
	origodficode varchar2(12),
	cnapsodfi varchar2(12),
	origtxtype varchar2(1),
	origtxssno varchar2(8),
	cancelstrinfo varchar2(60),
	cancelresult varchar2(1),
	resultstrinfo varchar2(60),
	procstate varchar2(2),
	statetime varchar2(14) 
)tablespace users;
create	 unique index mbfeoprtcancel_idx on mbfeoprtcancel( cancelno,consigndate,origodficode)tablespace indx;
