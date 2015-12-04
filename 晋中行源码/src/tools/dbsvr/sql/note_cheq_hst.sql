drop  table note_cheq_hst;
create table note_cheq_hst(
	ac_id number(9),
	ac_seq number(6),
	note_type varchar2(3),
	tx_br_no varchar2(5),
	beg_note_no varchar2(16),
	end_note_no varchar2(16),
	use_ind varchar2(1),
	cnt number(6),
	tx_date number(9),
	tx_time number(9),
	trace_no number(9),
	trace_cnt number(9),
	tel varchar2(4),
	chk varchar2(4),
	auth varchar2(4) 
)tablespace users;
