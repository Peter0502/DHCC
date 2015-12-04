drop  table mo_pay_again_ln;
create table mo_pay_again_ln(
	pact_no varchar2(16),
	amt number(16,2),
	pay_date number(9),
	intst_amt number(16,2),
	sts varchar2(1) 
)tablespace users;
