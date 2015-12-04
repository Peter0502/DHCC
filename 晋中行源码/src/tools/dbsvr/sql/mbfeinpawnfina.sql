drop index mbfeinpawnfina_idx;
drop  table mbfeinpawnfina;
create table mbfeinpawnfina(
	txssno varchar2(8),
	consigndate number(9),
	spjoincode varchar2(12),
	osdficodeccpc varchar2(4),
	osdficode varchar2(12),
	isdficodeccpc varchar2(4),
	isdficode varchar2(12),
	amount number(15,2),
	finaamt number(15,2),
	finarest number(15,2),
	procstate varchar2(2),
	checkstate varchar2(2),
	statetime varchar2(14),
	printno number(9),
	flowno number(9),
	errcode varchar2(8),
	errdesc varchar2(60),
	strinfo varchar2(60) 
)tablespace users;
create	 unique index mbfeinpawnfina_idx on mbfeinpawnfina( txssno,consigndate,spjoincode)tablespace indx;
