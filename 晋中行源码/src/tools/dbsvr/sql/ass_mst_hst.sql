drop  table ass_mst_hst;
create table ass_mst_hst(
	br_no varchar2(5),
	tx_date number(9),
	trace_no number(9),
	sub_br_no varchar2(5),
	pact_no number(9),
	tx_amt number(16,2),
	bal number(16,2),
	intst_acm number(19,2),
	tx_time number(9),
	brf varchar2(10),
	hst_cnt number(9),
	tel varchar2(4),
	chk varchar2(4),
	auth varchar2(4) 
)tablespace users;
