drop  table mo_rmny_smny;
create table mo_rmny_smny(
	br_no varchar2(5),
	occ_date number(9),
	seqn number(9),
	tx_opmd varchar2(1),
	cur_no varchar2(2),
	amt number(16,2),
	tx_date number(9),
	trace_no number(9),
	occ_tel varchar2(4),
	brf varchar2(40),
	tel varchar2(4),
	chk varchar2(4),
	auth varchar2(4),
	occ_br_no varchar2(5) 
)tablespace users;
