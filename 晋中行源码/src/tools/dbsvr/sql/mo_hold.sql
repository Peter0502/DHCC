drop  table mo_hold;
create table mo_hold(
	ac_id number(9),
	ac_seqn number(6),
	hold_seqn number(9),
	hold_typ varchar2(1),
	plan_hold_amt number(16,2),
	auto_unhold_ind varchar2(1),
	hold_sts varchar2(1),
	beg_hold_date number(9),
	plan_unhold_date number(9),
	fct_unhold_date number(9),
	hold_br_no varchar2(5),
	hold_tel varchar2(4),
	hold_brf varchar2(60),
	unhold_br_no varchar2(5),
	unhold_tel varchar2(4),
	unhold_brf varchar2(60),
	wrk_date number(9),
	trace_no number(9) 
)tablespace users;
