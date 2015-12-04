drop  table ln_paystatus;
create table ln_paystatus(
	ac_id number(9),
	ac_seqn number(6),
	pay_sts varchar2(24),
	clear_sts varchar2(1) 
)tablespace users;
