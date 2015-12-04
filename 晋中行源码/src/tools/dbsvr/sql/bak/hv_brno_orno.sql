drop index hv_brno_orno_idx;
drop  table hv_brno_orno;
create table hv_brno_orno(
	br_no varchar2(5),
	or_br_no varchar2(12),
	qs_no varchar2(12),
	br_name varchar2(60),
	or_br_name varchar2(60),
	qs_name varchar2(60) 
)tablespace users;
create	 unique index hv_brno_orno_idx on hv_brno_orno( br_no)tablespace indx;
