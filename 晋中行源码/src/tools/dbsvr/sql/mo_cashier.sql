drop  table mo_cashier;
create table mo_cashier(
	sign_br_no varchar2(5),
	cashier_che_no varchar2(16),
	tx_br_no varchar2(5),
	ct_ind varchar2(1),
	ration_ind varchar2(1),
	requ_name varchar2(50),
	requ_addr varchar2(50),
	requ_ac_id number(9),
	requ_ac_seqn number(6),
	requ_br_no varchar2(5),
	requ_br_name varchar2(40),
	sign_date number(9),
	payee_name varchar2(50),
	sign_amt number(16,2),
	purpose varchar2(40),
	rmk varchar2(40),
	cashier_sts varchar2(1),
	cashier_che_fag varchar2(1) 
)tablespace users;
