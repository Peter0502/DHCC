drop index mo_sxc_idx1;
drop table mo_sxc;
create table mo_sxc(
	ac_id number(9),
	ac_seqn number(6),
	bal number(16,2),
	intst number(16,2),
	un_intst number(16,2),
	tx_date number(9),
	trace_no number(9),
	untx_date number(9),
	untrace_no number(9),
	tx_br_no varchar2(5),
	tx_tel varchar2(4),
	untx_br_no varchar2(5),
	untx_tel varchar2(4),
	opn_br_no varchar2(5),
	sts varchar2(1),
	flag varchar2(1),
	qy_type varchar2(1)
)tablespace users;
create unique index mo_sxc_idx1 on mo_sxc(ac_id,ac_seqn,sts)tablespace indx;


