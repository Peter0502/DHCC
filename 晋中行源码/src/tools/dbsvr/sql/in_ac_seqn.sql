drop index in_ac_seqn_1;
drop  table in_ac_seqn;
create table in_ac_seqn(
	app_cod varchar2(4),
	seque number(9) 
)tablespace users;
create	 unique index in_ac_seqn_1 on in_ac_seqn( app_cod)tablespace indx;
