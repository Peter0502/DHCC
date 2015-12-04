drop  table ln_lo_rol;
create table ln_lo_rol(
	tx_date number(9),
	trace_no number(9),
	rol_ind varchar2(1),
	ac_id number(9),
	ac_seqn number(6),
	lo_term number(6),
	lo_amt number(16,2),
	lo_intst number(16,2),
	pun_intst number(16,2),
	pay_type varchar2(1),
	over_date number(9),
	shld_pay_date number(9),
	pay_lo_amt number(16,2),
	pay_lo_intst number(16,2),
	pay_pun_intst number(16,2),
	mac varchar2(16),
	old_ac_ind varchar2(1) 
)tablespace users;
