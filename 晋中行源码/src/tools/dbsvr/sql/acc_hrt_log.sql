/************科目日志表，用来存放没有分户帐的科目的交易历史*****/
/**drop index acc_hrt_log_idx;**/
drop index acc_hrt_log_idx1;
drop  table acc_hrt_log;
create table acc_hrt_log(
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
	bal number(16,2),
	tel varchar2(4),
	sts varchar2(1), 
	pagecnt number(9),
	print_flag varchar2(1) /**0 未打印 1 已经打印***/
)tablespace users;
create  index acc_hrt_log_idx1 on acc_hrt_log(print_flag,tx_opn_br_no,acc_hrt) tablespace indx;
/**create unique index acc_hrt_log_idx on acc_hrt_log(acc_hrt,dc_ind, tx_date,trace_no) tablespace indx;***/
