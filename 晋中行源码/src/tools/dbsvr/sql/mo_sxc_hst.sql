drop index mo_sxc_hst_idx1;
drop table mo_sxc_hst;
create table mo_sxc_hst(
	ac_id number(9),
	ac_seqn number(6),
	rate_date number(9),
	ic_date number(9),
	tx_date number(9),
	trace_no number(9),
	tx_amt number(16,2),
	hst_bal number(16,2),
	intst number(16,2),
	opn_br_no varchar2(5),
	tx_br_no varchar2(5),
	tx_tel varchar2(6),
	type varchar2(1),
	sts varchar2(1)
)tablespace users;
create	 unique index mo_sxc_hst_idx1 on mo_sxc_hst (ac_id,ac_seqn,rate_date,sts)tablespace indx;


