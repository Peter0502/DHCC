drop  table ln_pay_pln_rol;
create table ln_pay_pln_rol(
	tx_date number(9),
	trace_no number(9),
	rol_ind varchar2(1),
	ac_id number(9),
	ac_seqn number(6),
	de_ac_id number(9),
	de_ac_seqn number(6),
	ttl_amt number(16,2),
	ttl_cnt number(6),
	curr_cnt number(6),
	curr_amt number(16,2),
	curr_intst number(16,2),
	wrk_date number(9),
	beg_date number(9),
	end_date number(9),
	pay_ind varchar2(1),
	ic_date_aj number(9) 
)tablespace users;
