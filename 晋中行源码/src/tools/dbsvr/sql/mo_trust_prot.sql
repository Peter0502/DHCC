drop  table mo_trust_prot;
create table mo_trust_prot(
	trust_no varchar2(20),
	tx_br_no varchar2(5),
	cur_no varchar2(2),
	name varchar2(50),
	trust_amt number(16,2),
	out_amt number(16,2),
	afund_ac_id number(9),
	amt_ac_id number(9),
	intst_ac_id number(9),
	ln_ac_id number(9),
	ln_ac_seqn number(6),
	trust_ind varchar2(1) 
)tablespace users;
