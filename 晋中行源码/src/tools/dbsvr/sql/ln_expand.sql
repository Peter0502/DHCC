drop  table ln_expand;
create table ln_expand(
	ac_id number(9),
	ac_seqn number(6),
	exp_br_no varchar2(5),
	exp_amt number(12,2),
	exp_rate number(8,5),
	exp_beg_date number(9),
	exp_mtr_date number(9),
	exp_ind varchar2(1),
	rgst_date number(9),
	tel varchar2(4),
	trace_no number(9) 
)tablespace users;
