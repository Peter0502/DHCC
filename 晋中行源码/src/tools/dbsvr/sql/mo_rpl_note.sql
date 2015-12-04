drop  table mo_rpl_note;
create table mo_rpl_note(
	old_ac_no varchar2(19),
	br_no varchar2(5),
	mdm_code varchar2(4),
	old_note_no varchar2(16),
	new_ac_no varchar2(19),
	new_note_no varchar2(16),
	rpl_date number(9),
	tel varchar2(4),
	trace_no number(9),
	rmk varchar2(1) 
)tablespace users;
