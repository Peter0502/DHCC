drop  table ln_acm_reg;
create table ln_acm_reg(
	ac_id number(9),
	ac_seqn number(6),
	br_no varchar2(5),
	beg_date number(9),
	end_date number(9),
	cal_date number(9),
	rate number(8,5),
	intst_acm number(19,2),
	part_intst_acm number(19,2),
	intst number(16,2),
	intst_type varchar2(1),
	trace_no number(9),
	type varchar2(1) 
)tablespace users;
