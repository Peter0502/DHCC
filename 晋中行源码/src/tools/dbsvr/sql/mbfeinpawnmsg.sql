drop index mbfeinpawnmsg_idx;
drop  table mbfeinpawnmsg;
create table mbfeinpawnmsg(
	consigndate number(9),
	spjoincode varchar2(12),
	txssno varchar2(8),
	amount number(15,2),
	osdficodeccpc varchar2(4),
	osdficode varchar2(12),
	isdficodeccpc varchar2(4),
	isdficode varchar2(12),
	procstate varchar2(2),
	checkstate varchar2(2),
	statetime varchar2(14),
	printno number(9),
	flowno number(9),
	errcode varchar2(8),
	errdesc varchar2(60),
	strinfo varchar2(60) 
)tablespace users;
create	 unique index mbfeinpawnmsg_idx on mbfeinpawnmsg( consigndate,spjoincode,txssno)tablespace indx;
