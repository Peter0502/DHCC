drop index mo_qzgl_idx1;
drop table mo_qzgl;
create table mo_qzgl(
	child_ac_id number(9),
	part_ac_id number(9),
	tx_date number(9),
	tx_br_no varchar2(5),
	trace_no number(9),
	tx_tel varchar2(4),
	untx_date number(9),
	untx_br_no varchar2(5),
	untrace_no number(9),
	untx_tel varchar2(4),
	qy_amt number(16,2),
	sts varchar2(1)
	hz_date number(3),
)tablespace users;
--create unique index mo_qzgl_idx1 on mo_qzgl(child_ac_id,part_ac_id,sts);
