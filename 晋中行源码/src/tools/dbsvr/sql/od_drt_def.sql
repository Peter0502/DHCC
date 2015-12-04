drop index od_drt_def_1;
drop  table od_drt_def;
create table od_drt_def(
	prdt_code varchar2(3),
	tx_code varchar2(4),
	drt_ratio number(8,5) 
)tablespace users;
create	 unique index od_drt_def_1 on od_drt_def( prdt_code,tx_code)tablespace indx;
