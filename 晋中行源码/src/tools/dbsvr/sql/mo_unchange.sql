drop  table mo_unchange;
create table mo_unchange(
	ac_id number(9),
	ac_seqn number(6),
	in_acno varchar2(19),
	bal number(16,2),
	intst number(16,2),
	ind varchar2(2),
	prdt_no varchar2(5),
	trace_no number(9),
	tx_br_no varchar2(5),
	tx_date number(9),
	tx_time number(9),
	brf varchar2(30),
	tel varchar2(4) 
)tablespace users;
