drop index mbfebksttalt_idx;
drop  table mbfebksttalt;
create table mbfebksttalt(
	consigndate number(9),
	txssno varchar2(8),
	amount number(15,2),
	srflag varchar2(1),
	odficode varchar2(12),
	rdficode varchar2(12),
	oprttype varchar2(2),
	warrantno varchar2(8),
	compensationamnt number(7,4),
	tel varchar2(4),
	tx_date number(9),
	tx_time number(9),
	strinfo varchar2(60) 
)tablespace users;
create	 unique index mbfebksttalt_idx on mbfebksttalt( consigndate,txssno,odficode)tablespace indx;
