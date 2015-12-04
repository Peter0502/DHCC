drop table mo_sxc_hst_intst;
create table mo_sxc_hst_intst(
	ac_id number(9),
	ac_seqn number(6),
	rate_date number(9),
	ic_date number(9),
	tx_date number(9),
	acct_date number(9),
	trace_no number(9),
	days number(6),
	tx_amt number(16,2),
	hst_bal number(16,2),
	intst number(16,2),
	rate number(16,3),
	rate_code varchar2(3),
	opn_br_no varchar2(5),
	tx_br_no varchar2(5),
	tx_tel varchar2(6),
	type varchar2(1),
	sts varchar2(1)
)tablespace users;
