drop index mo_bank_po_query_idx;
drop  table mo_bank_po_query;
create table mo_bank_po_query(
	tx_date number(9),
	trace_no number(9),
	tx_br_no varchar2(5),
	query_date number(9),
	query_br_no varchar2(12),
	po_no varchar2(16),
	query_brf varchar2(50),
	sts varchar2(1) 
)tablespace users;
create	 unique index mo_bank_po_query_idx on mo_bank_po_query( tx_date,trace_no)tablespace indx;
