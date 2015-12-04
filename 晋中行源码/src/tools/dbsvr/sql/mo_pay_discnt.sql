drop  table mo_pay_discnt;
create table mo_pay_discnt(
	pact_no varchar2(20),
	tpact_no varchar2(20),
	rpact_no varchar2(20),
	ac_id number(9),
	ac_seqn number(6),
	ct_ind varchar2(1),
	pay_amt number(16,2),
	turn_ac_no varchar2(24),
	turn_name varchar2(50),
	ac_type varchar2(1),
	turn_bal number(16,2),
	in_ac_no varchar2(10),
	tin_ac_no varchar2(10),
	rin_ac_no varchar2(10),
	sts varchar2(1),
	pn_type varchar2(1) 
)tablespace users;
