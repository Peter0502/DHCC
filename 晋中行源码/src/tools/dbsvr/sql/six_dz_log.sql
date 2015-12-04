drop  table six_dz_log;
create table six_dz_log(
	inf_source varchar2(1),
	tx_date number(9),
	tx_code varchar2(4),
	ac_no varchar2(20),
	sl_bank varchar2(10),
	mb_bank varchar2(10),
	tx_amt number(16,2),
	sl_trace_no varchar2(10),
	center_trace_no varchar2(10),
	opn_trace_no varchar2(10) 
)tablespace users;
