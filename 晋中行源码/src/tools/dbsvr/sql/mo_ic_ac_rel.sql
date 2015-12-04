drop index mo_ic_ac_rel_1;
drop  table mo_ic_ac_rel;
create table mo_ic_ac_rel(
	sub_seqn number(4),
	main_ac_no varchar2(20),
	app_ac_no varchar2(20),
	tx_date number(9),
	sts varchar2(1)
)tablespace users;
create	 unique index mo_ic_ac_rel_1 on mo_ic_ac_rel(main_ac_no,app_ac_no,sub_seqn)tablespace indx;