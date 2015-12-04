drop index od_dd_rel_1;
drop  table od_dd_rel;
create table od_dd_rel(
	dd_prdt_code varchar2(3),
	od_prdt_code varchar2(3) 
)tablespace users;
create	 unique index od_dd_rel_1 on od_dd_rel( dd_prdt_code,od_prdt_code)tablespace indx;
