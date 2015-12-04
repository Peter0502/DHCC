drop  table ln_gage_reg;
create table ln_gage_reg(
	pact_no varchar2(20),
	name varchar2(50),
	ac_id number(9),
	ac_seqn number(6),
	gage_type varchar2(2),
	amt number(16,2),
	gage_inf varchar2(50),
	sts varchar2(1),
	in_tx_date number(9),
	in_trace_no number(9),
	out_tx_date number(9),
	out_trace_no number(9),
	ln_bal number(16,2),
	estival number(16,2),
	tel varchar(4)
)tablespace users;
