drop index trace_log_rz2;
drop index trace_log_rz_idx1;
drop index trace_log_rz_idx2;
drop  table trace_log_rz;
create table trace_log_rz(
	trace_no number(9),
	trace_cnt number(9),
	tx_date number(9),
	tx_time number(9),
	tx_br_no varchar2(5),
	opn_br_no varchar2(5),
	tx_code varchar2(4),
	sub_tx_code varchar2(4),
	svc_ind number(6),
	cur_no varchar2(2),
	prdt_no varchar2(3),
	ac_no varchar2(19),
	ac_seqn number(6),
	ac_id number(9),
	note_type varchar2(3),
	note_no varchar2(16),
	add_ind varchar2(1),
	amt number(16,2),
	ct_ind varchar2(1),
	brf varchar2(20),
	tel varchar2(4),
	chk varchar2(4),
	aut varchar2(4),
	hst_ind varchar2(1),
	saving_notarize varchar2(1),
	no_show varchar2(1),
	sts varchar2(1)
)tablespace users;
create	 index trace_log_rz2 on trace_log_rz( tx_date,tx_br_no,add_ind,amt)tablespace indx;
create	 index trace_log_rz_idx1 on trace_log_rz( tx_date,trace_no)tablespace indx;
create   index trace_log_rz_idx2 on trace_log_rz(tel, tx_date)tablespace indx;
create   index trace_log_rz_idx3 on trace_log_rz(brf,sts,tx_br_no,tx_date)tablespace indx;
