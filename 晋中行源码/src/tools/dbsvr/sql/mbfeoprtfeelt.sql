drop  table mbfeoprtfeelt;
create table mbfeoprtfeelt(
	row_id number(9),
	consigndate number(9),
	paytools varchar2(1),
	cmtno varchar2(3),
	srflag varchar2(1),
	feeptmlow varchar2(20),
	feeptmupp varchar2(20),
	feecnt varchar2(10),
	feeprice number(6,2),
	feerate number(6,2),
	timespec number(6,4),
	feeamnt number(15,2) 
)tablespace users;
