drop  table mo_po_qry;
create table mo_po_qry(
	po_no varchar2(16),
	qry_typ varchar2(1),
	tx_date number(9),
	brf varchar2(200),
	sts varchar2(1),
	tel varchar2(5),
	br_no varchar2(5) 
)tablespace users;
