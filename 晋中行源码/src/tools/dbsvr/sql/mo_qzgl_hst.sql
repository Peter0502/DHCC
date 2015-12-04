drop index mo_qzgl_hst_1;
drop table mo_qzgl_hst;
create table mo_qzgl_hst(
	child_ac_id number(9),
	part_ac_id  number(9),
	tx_br_no    varchar2(60),
	trace_no    number(9),
	tx_date     number(9),
	tx_amt      number(16,2),
	tx_tel      varchar2(4),
	child_bal   number(16,2)
)tablespace users;
create index mo_qzgl_hst_1 on mo_qzgl_hst(child_ac_id,part_ac_id,tx_date);
