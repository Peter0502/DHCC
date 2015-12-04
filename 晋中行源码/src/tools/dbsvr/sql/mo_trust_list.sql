drop  table mo_trust_list;
create table mo_trust_list(
	turst_pay_no varchar2(20),
	payer_seqn number(6),
	payer_ac_no varchar2(24),
	payer_name varchar2(50),
	payer_br_no varchar2(5),
	payer_br_name varchar2(40),
	amt number(16,2),
	sts varchar2(1) 
)tablespace users;
