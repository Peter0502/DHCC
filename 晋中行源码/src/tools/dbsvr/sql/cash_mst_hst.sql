drop  table cash_mst_hst;
create table cash_mst_hst(
	trace_no number(9),
	trace_cnt number(9),
	br_no varchar2(5),
	ac_no varchar2(10),
	tel varchar2(4),
	cur_no varchar2(2),
	add_ind varchar2(1),
	tw_ac_no varchar2(24),
	amt number(16,2),
	tx_date number(9),
	tx_time number(9),
	brf varchar2(20),
	cash_code varchar2(20) 
)tablespace users;
