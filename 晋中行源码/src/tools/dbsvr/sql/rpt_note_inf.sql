drop  table rpt_note_inf;
create table rpt_note_inf(
	tx_date number(8),
	trace_no number(9),
	opn_br_no varchar2(9),
	tx_br_no varchar2(9),
	tx_opn_br_no varchar2(9),
	tx_tx_br_no varchar2(9),
	dc_ind varchar2(1),
	acc_hrt varchar2(7),
	tw_acchrt varchar2(7),
	ac_no varchar2(19),
	name varchar2(40),
	amt number(16,2),
	cur_no varchar2(2),
	txcode varchar2(4),
	summary varchar2(40) 
)tablespace users;
