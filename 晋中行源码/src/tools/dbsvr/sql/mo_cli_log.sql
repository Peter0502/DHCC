drop  table mo_cli_log;
create table mo_cli_log(
	br_no varchar2(5),
	ac_id number(9),
	ac_seqn number(6),
	"date" number(9),
	time number(9),
	dc_ind varchar2(1),
	amt number(12,2),
	note_knd varchar2(3),
	note_no varchar2(16),
	ct_ind varchar2(1) 
)tablespace users;
