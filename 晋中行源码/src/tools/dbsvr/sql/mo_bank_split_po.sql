drop  table mo_bank_split_po;
create table mo_bank_split_po(
	pact_no varchar2(12),
	tx_br_no varchar2(5),
	req_date number(9),
	payer_name varchar2(50),
	payer_ac_no varchar2(32),
	payer_br_name varchar2(40),
	payee_name varchar2(50),
	payee_ac_no varchar2(32),
	payee_br_name varchar2(40),
	pnote_no varchar2(12),
	par_amt number(16,2),
	matr_date number(9),
	bail_ac_no varchar2(24),
	pay_type varchar2(1),
	sts varchar2(1) 
)tablespace users;
