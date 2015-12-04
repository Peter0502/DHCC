drop  table mo_fet_cash;
create table mo_fet_cash(
	ac_id number(9),
	ac_seqn number(6),
	tx_date number(9),
	amt number(16,2),
	trace_no number(9),
	type varchar2(1) 
)tablespace users;
