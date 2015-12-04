drop  table ln_eod_pri;
create table ln_eod_pri(
	ln_ac_no varchar2(20),
	ln_ac_seqn number(6),
	ln_ac_name varchar2(50),
	ln_pact_no varchar2(20),
	ln_ac_prdt varchar2(3),
	pay_ac_no varchar2(20),
	pay_ac_seqn number(6),
	pay_ac_name varchar2(50),
	amt number(16,2),
	acm number(19,2),
	rate number(8,5),
	over_rate number(8,5),
	tx_code varchar2(4),
	amt_type varchar2(1),
	acm_type varchar2(1),
	beg_date number(9),
	end_date number(9),
	trace_no number(9),
	tx_date number(9) 
)tablespace users;
