drop index com_parm_1;
drop  table com_parm;
create table com_parm(
	parm_code varchar2(5),
	parm_seqn number(6),
	parm_name varchar2(40),
	val_type varchar2(1),
	val varchar2(20) 
)tablespace users;
create	 unique index com_parm_1 on com_parm( parm_code,parm_seqn)tablespace indx;
