drop index new_old_accno_1;
drop  table new_old_accno;
create table new_old_accno(
	old_acchrt varchar2(5),
	new_ysx_acchrt varchar2(5),
	new_bwx_acchrt varchar2(5),
	new_lxsr_acchrt varchar2(5)
)tablespace users;
create	 unique index new_old_accno_1 on new_old_accno(old_acchrt );
