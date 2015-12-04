drop  table com_sys_parm;
create table com_sys_parm(
	sys_sts number(6),
	lst_date number(9),
	sys_date number(9),
	real_time_ind varchar2(1),
	sys_ind varchar2(10) 
)tablespace users;
