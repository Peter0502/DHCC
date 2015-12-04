drop  table up_dd_mst_hst;
create table up_dd_mst_hst(
	trace_no number(9),
	trace_cnt number(9),
	opn_br_no varchar2(5),
	tx_br_no varchar2(5),
	ac_no varchar2(19),
	sub_ac_no varchar2(19),
	add_ind varchar2(1),
	ct_ind varchar2(1),
	tx_amt number(16,2),
	bal number(16,2),
	intst_acm number(19,2),
	tx_date number(9),
	tx_time number(9),
	hst_cnt number(9),
	brf varchar2(20),
	tel varchar2(4),
	chk varchar2(4),
	auth varchar2(4) 
)tablespace users;
