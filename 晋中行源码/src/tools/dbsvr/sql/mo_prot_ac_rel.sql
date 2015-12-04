drop index mo_prot_ac_rel_idx;
drop  table mo_prot_ac_rel;
create table mo_prot_ac_rel(
	tx_br_no varchar2(5),
	prot_type varchar2(3),
	main_ac_id number(9),
	main_ac_seqn number(6),
	app_ac_id number(9),
	app_ac_seqn number(6),
	tx_date number(9),
	trace_no number(9),
	tel varchar2(4),
	sts varchar2(1) 
)tablespace users;
create	 index mo_prot_ac_rel_idx on mo_prot_ac_rel( main_ac_id,main_ac_seqn)tablespace indx;
