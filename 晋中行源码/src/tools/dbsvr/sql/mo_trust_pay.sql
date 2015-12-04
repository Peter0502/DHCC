drop  table mo_trust_pay;
create table mo_trust_pay(
	br_no varchar2(5),
	turst_pay_no varchar2(10),
	turst_fag varchar2(1),
	requ_name varchar2(50),
	requ_id_type varchar2(1),
	requ_id_no varchar2(20),
	payee_ac_id number(9),
	payee_ac_seqn number(6),
	sts varchar2(1) 
)tablespace users;
