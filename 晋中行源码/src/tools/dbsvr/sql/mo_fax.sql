drop  table mo_fax;
create table mo_fax(
	tx_date number(9),
	tx_br_no varchar2(5),
	fax_no varchar2(16),
	requ_name varchar2(50),
	payer_br_no varchar2(5),
	payer_br_name varchar2(40),
	payer_ac_id number(9),
	payer_ac_seqn number(6),
	payer_name varchar2(50),
	payee_br_no varchar2(5),
	payee_br_name varchar2(40),
	payee_ac_no varchar2(24),
	payee_name varchar2(50),
	fax_amt number(16,2),
	fax_sts varchar2(1),
	pr_cnt number(6) 
)tablespace users;
