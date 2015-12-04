drop index hv_orno_brno_idx;
drop  table hv_orno_brno;
create table hv_orno_brno(
	or_br_no varchar2(12),
	br_no varchar2(5),
	qs_no varchar2(12),
	br_name varchar2(60),
	or_br_name varchar2(60),
	qs_name varchar2(60) 
)tablespace users;
create	 unique index hv_orno_brno_idx on hv_orno_brno( or_br_no)tablespace indx;
