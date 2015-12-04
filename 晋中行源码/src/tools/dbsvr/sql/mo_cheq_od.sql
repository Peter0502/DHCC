drop  table mo_cheq_od;
create table mo_cheq_od(
	ac_id number(9),
	ac_seqn number(6),
	note_type varchar2(3),
	note_no varchar2(16),
	brf varchar2(40),
	tel varchar2(4),
	tx_date number(9),
	trace_no number(9) 
)tablespace users;
