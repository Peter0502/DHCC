drop  table dc_log_bk;
create table dc_log_bk(
	tx_date number(9),
	trace_no number(9),
	trace_cnt number(9),
	entry_code varchar2(6),
	dc_seqn number(6),
	opn_br_no varchar2(5),
	tx_br_no varchar2(5),
	tx_opn_br_no varchar2(5),
	tx_tx_br_no varchar2(5),
	cur_no varchar2(2),
	acc_hrt varchar2(5),
	dc_ind varchar2(1),
	ct_ind varchar2(1),
	note_type varchar2(3),
	note_no varchar2(16),
	amt number(16,2),
	tel varchar2(4),
	sts varchar2(1) 
)tablespace users;
