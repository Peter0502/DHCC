drop  table chnl_single_hold;
create table chnl_single_hold(
	tx_date number(8),
	tx_time varchar2(6),
	trace_no number(6),
	cur_no varchar2(2),
	ac_no varchar2(32),
	tx_code varchar2(4),
	tx_br_no varchar2(5),
	opn_br_no varchar2(5),
	tel varchar2(6),
	bbmk varchar2(4),
	ttyn varchar2(20),
	pt_date number(8),
	pt_time varchar2(6),
	pt_trace_no varchar2(12),
	premise_no varchar2(10),
	hold_amt number(17,2),
	tx_amt number(17,2),
	brf varchar2(30),
	sts varchar2(1),
	o_tx_date number(8),
	o_trace_no varchar2(12),
	o_pt_date number(8),
	o_pt_trace_no varchar2(12),
	dz_ind varchar2(1),
	dz_ref varchar2(28)
)tablespace users;
create unique index chnl_single_hold_1 on chnl_single_hold
(bbmk,pt_date,pt_trace_no,tx_date,trace_no);
