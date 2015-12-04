drop index tel_ac;
drop  table tel_save_inf;
create table tel_save_inf(
	ac_no varchar2(19),
	ac_seqn number(6),
	tel varchar2(4) 
)tablespace users;
create	 unique index tel_ac on tel_save_inf( ac_no,ac_seqn,tel)tablespace indx;
