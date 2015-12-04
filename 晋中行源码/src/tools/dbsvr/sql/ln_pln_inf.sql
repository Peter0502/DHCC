drop  table ln_pln_inf;
create table ln_pln_inf(
	ac_id number(9),
	ac_seqn number(6),
	br_no varchar2(5),
	curr_amt number(16,2),
	true_amt number(16,2),
	wrk_date number(9),
	beg_date number(9),
	end_date number(9),
	lst_date number(9),
	pay_ind varchar2(1),
	content varchar2(24) 
)tablespace users;
