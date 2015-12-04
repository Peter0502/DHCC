drop index ln_mst_hst_ind;
drop  table ln_mst_hst;
create table ln_mst_hst(
	trace_no number(9),
	trace_cnt number(9),
	opn_br_no varchar2(5),
	tx_br_no varchar2(5),
	ac_id number(9),
	ac_seqn number(6),
	tx_code varchar2(4),
	sub_tx_code varchar2(4),
	ln_tx_type varchar2(1),
	add_ind varchar2(1),
	ct_ind varchar2(1),
	tx_amt number(16,2),
	bal number(16,2),
	intst_acm number(19,2),
	tx_date number(9),
	tx_time number(9),
	note_type varchar2(3),
	note_no varchar2(16),
	hst_cnt number(9),
	brf varchar2(20),
	tel varchar2(4),
	chk varchar2(4),
	auth varchar2(4) 
)tablespace users;
create	 index ln_mst_hst_ind on ln_mst_hst( ac_id,ac_seqn)tablespace indx;
