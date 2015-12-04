drop  table mo_again_ln;
create table mo_again_ln(
	pact_no varchar2(16),
	ac_id number(9),
	br_no varchar2(5),
	prdt_no varchar2(3),
	amt number(16,2),
	cur_amt number(16,2),
	sum_amt number(16,2),
	rate number(8,6),
	limit number(6),
	to_date number(9),
	reg_date number(9),
	pay_date number(9),
	tel varchar2(4),
	chk varchar2(4),
	auth varchar2(4),
	again_ln_type varchar2(1),
	sts varchar2(1) 
)tablespace users;
