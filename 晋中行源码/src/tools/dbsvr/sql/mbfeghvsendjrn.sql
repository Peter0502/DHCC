drop index mbfeghvsendjrn_idx;
drop  table mbfeghvsendjrn;
create table mbfeghvsendjrn(
	tx_br_no varchar2(5),
	tx_code varchar2(4),
	consigndate number(9),
	trace_no number(9),
	procstate varchar2(1),
	payeracc varchar2(32),
	amount number(15,2),
	payername varchar2(60) 
)tablespace users;
create	 unique index mbfeghvsendjrn_idx on mbfeghvsendjrn( trace_no,consigndate)tablespace indx;
