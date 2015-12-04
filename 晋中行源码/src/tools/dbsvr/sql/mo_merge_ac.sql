drop index mo_merge_ac_1;
drop  table mo_merge_ac;
create table mo_merge_ac(
	old_br_no varchar2(7),
	old_ac_no varchar2(20),
	br_no varchar2(5),
	ac_no varchar2(19),
	ac_id number(9),
	ac_seqn number(6),
	chg_mdm_yn varchar2(1) 
)tablespace users;
create	 index mo_merge_ac_1 on mo_merge_ac( old_ac_no)tablespace indx;
create   index mo_merge_ac_2 on mo_merge_ac(ac_no) tablespace indx;
create   index mo_merge_ac_3 on mo_merge_ac(ac_id) tablespace indx;
