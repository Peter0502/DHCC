drop  table whwtsk_reg;
create table whwtsk_reg(
	pact_no varchar2(20),
	tx_br_no varchar2(5),
	req_date number(9),
	payer_name varchar2(50),
	payer_ac_no varchar2(24),
	payer_br_no varchar2(5),
	payer_br_name varchar2(40),
	payee_name varchar2(50),
	payee_ac_no varchar2(32),
	payee_br_name varchar2(40),
	sign_amt number(16,2),
	matr_date number(9),
	bail_ac_no varchar2(24),
	bail_amt number(16,2),
	bail_amt_ratio number(12,2),
	pact_num varchar2(12),
	tot_mort_amt number(16,2),
	advance_amt number(16,2),
	over_ac_no varchar2(24),
	over_rate number(8,5),
	acc_po_sts varchar2(1),
	acc_po_ind varchar2(1),
	acc_type varchar2(1) 
)tablespace users;
create   unique index whwtsk_reg_1 on whwtsk_reg( pact_no)tablespace indx;
